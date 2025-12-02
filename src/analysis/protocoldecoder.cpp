#include "protocoldecoder.h"
#include <cmath>
#include <algorithm>

ProtocolDecoder::ProtocolDecoder(QObject *parent)
    : QObject(parent)
{
}

void ProtocolDecoder::setProtocol(Protocol protocol)
{
    if (m_protocol != protocol) {
        m_protocol = protocol;
        emit protocolChanged(protocol);
    }
}

void ProtocolDecoder::setUARTBaudRate(int baudRate) { m_uartBaudRate = baudRate; }
void ProtocolDecoder::setUARTDataBits(int bits) { m_uartDataBits = bits; }
void ProtocolDecoder::setUARTStopBits(int bits) { m_uartStopBits = bits; }
void ProtocolDecoder::setUARTParity(int parity) { m_uartParity = parity; }
void ProtocolDecoder::setUARTPolarity(bool inverted) { m_uartInverted = inverted; }

void ProtocolDecoder::setSPIClockPolarity(int cpol) { m_spiCPOL = cpol; }
void ProtocolDecoder::setSPIClockPhase(int cpha) { m_spiCPHA = cpha; }
void ProtocolDecoder::setSPIBitOrder(bool msbFirst) { m_spiMSBFirst = msbFirst; }
void ProtocolDecoder::setSPIWordSize(int bits) { m_spiWordSize = bits; }

void ProtocolDecoder::setI2CAddressMode(int bits) { m_i2cAddressMode = bits; }

void ProtocolDecoder::setHighThreshold(double threshold) { m_highThreshold = threshold; }
void ProtocolDecoder::setLowThreshold(double threshold) { m_lowThreshold = threshold; }

QVector<ProtocolDecoder::DecodedFrame> ProtocolDecoder::decode(
    const QVector<QPointF> &dataChannel,
    const QVector<QPointF> &clockChannel,
    const QVector<QPointF> &csChannel)
{
    QVector<DecodedFrame> frames;
    
    switch (m_protocol) {
        case Protocol::UART:
            frames = decodeUART(dataChannel);
            break;
        case Protocol::I2C:
            if (!clockChannel.isEmpty()) {
                frames = decodeI2C(dataChannel, clockChannel);
            }
            break;
        case Protocol::SPI:
            // SPI needs MOSI, MISO, SCK, CS
            break;
        default:
            break;
    }
    
    emit decodingComplete(frames);
    return frames;
}

QVector<ProtocolDecoder::DecodedFrame> ProtocolDecoder::decodeI2C(
    const QVector<QPointF> &sda,
    const QVector<QPointF> &scl)
{
    QVector<DecodedFrame> frames;
    
    if (sda.isEmpty() || scl.isEmpty()) {
        return frames;
    }
    
    double threshold = (m_highThreshold + m_lowThreshold) / 2.0;
    
    // Find START and STOP conditions
    // START: SDA falls while SCL is high
    // STOP: SDA rises while SCL is high
    
    int i = 1;
    while (i < sda.size() && i < scl.size()) {
        bool sdaPrev = sda[i-1].y() > threshold;
        bool sdaCurr = sda[i].y() > threshold;
        bool sclCurr = scl[i].y() > threshold;
        
        // START condition
        if (sdaPrev && !sdaCurr && sclCurr) {
            DecodedFrame startFrame;
            startFrame.startTime = sda[i].x();
            startFrame.type = "START";
            startFrame.description = "Start condition";
            frames.append(startFrame);
            
            // Read address byte
            QByteArray addrByte;
            int bitCount = 0;
            uint8_t byte = 0;
            
            i++;
            while (i < scl.size() && bitCount < 8) {
                // Wait for SCL rising edge
                if (i > 0 && scl[i-1].y() <= threshold && scl[i].y() > threshold) {
                    // Sample SDA
                    bool bit = sda[i].y() > threshold;
                    byte = (byte << 1) | (bit ? 1 : 0);
                    bitCount++;
                }
                i++;
            }
            
            if (bitCount == 8) {
                uint8_t addr = byte >> 1;
                bool rw = byte & 0x01;
                
                DecodedFrame addrFrame;
                addrFrame.startTime = startFrame.startTime;
                addrFrame.endTime = sda[i].x();
                addrFrame.type = "ADDRESS";
                addrFrame.data.append(byte);
                addrFrame.description = QString("Addr: 0x%1 %2")
                                        .arg(addr, 2, 16, QChar('0'))
                                        .arg(rw ? "READ" : "WRITE");
                frames.append(addrFrame);
                
                // Read ACK/NAK
                while (i < scl.size()) {
                    if (i > 0 && scl[i-1].y() <= threshold && scl[i].y() > threshold) {
                        bool ack = !(sda[i].y() > threshold);
                        
                        DecodedFrame ackFrame;
                        ackFrame.startTime = sda[i].x();
                        ackFrame.type = ack ? "ACK" : "NAK";
                        ackFrame.description = ack ? "Acknowledge" : "Not Acknowledge";
                        frames.append(ackFrame);
                        i++;
                        break;
                    }
                    i++;
                }
            }
            
            // Continue reading data bytes
            while (i < sda.size() - 1) {
                sdaPrev = sda[i-1].y() > threshold;
                sdaCurr = sda[i].y() > threshold;
                sclCurr = scl[i].y() > threshold;
                
                // Check for STOP
                if (!sdaPrev && sdaCurr && sclCurr) {
                    DecodedFrame stopFrame;
                    stopFrame.startTime = sda[i].x();
                    stopFrame.type = "STOP";
                    stopFrame.description = "Stop condition";
                    frames.append(stopFrame);
                    break;
                }
                
                // Check for repeated START
                if (sdaPrev && !sdaCurr && sclCurr) {
                    break;  // Will be caught in next iteration
                }
                
                // Read data byte
                bitCount = 0;
                byte = 0;
                double byteStart = sda[i].x();
                
                while (i < scl.size() && bitCount < 8) {
                    if (i > 0 && scl[i-1].y() <= threshold && scl[i].y() > threshold) {
                        bool bit = sda[i].y() > threshold;
                        byte = (byte << 1) | (bit ? 1 : 0);
                        bitCount++;
                    }
                    i++;
                }
                
                if (bitCount == 8) {
                    DecodedFrame dataFrame;
                    dataFrame.startTime = byteStart;
                    dataFrame.endTime = sda[i].x();
                    dataFrame.type = "DATA";
                    dataFrame.data.append(byte);
                    dataFrame.description = QString("0x%1 '%2'")
                                           .arg(byte, 2, 16, QChar('0'))
                                           .arg(QChar(byte).isPrint() ? QChar(byte) : '.');
                    frames.append(dataFrame);
                    
                    // Read ACK/NAK
                    while (i < scl.size()) {
                        if (i > 0 && scl[i-1].y() <= threshold && scl[i].y() > threshold) {
                            bool ack = !(sda[i].y() > threshold);
                            
                            DecodedFrame ackFrame;
                            ackFrame.startTime = sda[i].x();
                            ackFrame.type = ack ? "ACK" : "NAK";
                            frames.append(ackFrame);
                            i++;
                            break;
                        }
                        i++;
                    }
                }
            }
        }
        i++;
    }
    
    return frames;
}

QVector<ProtocolDecoder::DecodedFrame> ProtocolDecoder::decodeUART(
    const QVector<QPointF> &data)
{
    QVector<DecodedFrame> frames;
    
    if (data.isEmpty()) {
        return frames;
    }
    
    double bitPeriod = 1.0 / m_uartBaudRate;
    double threshold = (m_highThreshold + m_lowThreshold) / 2.0;
    
    // Find edges
    QVector<Edge> edges = findEdges(data, threshold);
    
    if (edges.isEmpty()) {
        return frames;
    }
    
    // UART idle is high (for non-inverted)
    bool idleLevel = !m_uartInverted;
    
    int edgeIdx = 0;
    while (edgeIdx < edges.size()) {
        // Look for start bit (transition from idle to active)
        if ((idleLevel && !edges[edgeIdx].rising) || 
            (!idleLevel && edges[edgeIdx].rising)) {
            
            double startTime = edges[edgeIdx].time;
            
            // Sample data bits in the middle of each bit period
            uint16_t byte = 0;
            int validBits = 0;
            bool parityBit = false;
            bool frameError = false;
            
            for (int bit = 0; bit < m_uartDataBits; ++bit) {
                double sampleTime = startTime + bitPeriod * (1.5 + bit);
                bool level = sampleLevel(data, sampleTime, threshold);
                if (m_uartInverted) level = !level;
                
                byte |= (level ? 1 : 0) << bit;
                validBits++;
            }
            
            // Sample parity bit if enabled
            if (m_uartParity != 0) {
                double sampleTime = startTime + bitPeriod * (1.5 + m_uartDataBits);
                parityBit = sampleLevel(data, sampleTime, threshold);
                if (m_uartInverted) parityBit = !parityBit;
                
                // Check parity
                int ones = 0;
                for (int bit = 0; bit < m_uartDataBits; ++bit) {
                    if (byte & (1 << bit)) ones++;
                }
                if (parityBit) ones++;
                
                bool parityOk = (m_uartParity == 1) ? (ones % 2 == 1) : (ones % 2 == 0);
                if (!parityOk) {
                    frameError = true;
                }
            }
            
            // Sample stop bit(s)
            double stopTime = startTime + bitPeriod * (1.5 + m_uartDataBits + 
                              (m_uartParity != 0 ? 1 : 0));
            bool stopBit = sampleLevel(data, stopTime, threshold);
            if (m_uartInverted) stopBit = !stopBit;
            
            if (!stopBit) {
                frameError = true;
            }
            
            // Create frame
            DecodedFrame frame;
            frame.startTime = startTime;
            frame.endTime = startTime + bitPeriod * (1 + m_uartDataBits + 
                           (m_uartParity != 0 ? 1 : 0) + m_uartStopBits);
            frame.type = "DATA";
            frame.data.append(static_cast<char>(byte));
            
            if (byte >= 32 && byte < 127) {
                frame.description = QString("0x%1 '%2'")
                                   .arg(byte, 2, 16, QChar('0'))
                                   .arg(QChar(byte));
            } else {
                frame.description = QString("0x%1").arg(byte, 2, 16, QChar('0'));
            }
            
            frame.error = frameError;
            if (frameError) {
                frame.errorMessage = "Framing/Parity error";
            }
            
            frames.append(frame);
            
            // Move past this byte
            while (edgeIdx < edges.size() && edges[edgeIdx].time < frame.endTime) {
                edgeIdx++;
            }
        } else {
            edgeIdx++;
        }
    }
    
    return frames;
}

QVector<ProtocolDecoder::DecodedFrame> ProtocolDecoder::decodeSPI(
    const QVector<QPointF> &mosi,
    const QVector<QPointF> &miso,
    const QVector<QPointF> &sck,
    const QVector<QPointF> &cs)
{
    QVector<DecodedFrame> frames;
    
    if (sck.isEmpty()) {
        return frames;
    }
    
    double threshold = (m_highThreshold + m_lowThreshold) / 2.0;
    
    // Find clock edges
    QVector<Edge> clockEdges = findEdges(sck, threshold);
    
    // Determine which edge to sample on based on CPOL/CPHA
    bool sampleOnRising = (m_spiCPOL == 0 && m_spiCPHA == 0) ||
                          (m_spiCPOL == 1 && m_spiCPHA == 1);
    
    int bitCount = 0;
    uint32_t mosiWord = 0;
    uint32_t misoWord = 0;
    double wordStart = 0;
    
    for (const Edge &edge : clockEdges) {
        // Check if CS is active (low)
        if (!cs.isEmpty()) {
            bool csActive = !sampleLevel(cs, edge.time, threshold);
            if (!csActive) {
                if (bitCount > 0) {
                    // End of transfer
                    DecodedFrame frame;
                    frame.startTime = wordStart;
                    frame.endTime = edge.time;
                    frame.type = "DATA";
                    frame.description = QString("MOSI: 0x%1  MISO: 0x%2")
                                       .arg(mosiWord, m_spiWordSize/4, 16, QChar('0'))
                                       .arg(misoWord, m_spiWordSize/4, 16, QChar('0'));
                    frames.append(frame);
                    
                    bitCount = 0;
                    mosiWord = 0;
                    misoWord = 0;
                }
                continue;
            }
        }
        
        // Sample on appropriate edge
        if ((sampleOnRising && edge.rising) || (!sampleOnRising && !edge.rising)) {
            if (bitCount == 0) {
                wordStart = edge.time;
            }
            
            bool mosiBit = mosi.isEmpty() ? false : sampleLevel(mosi, edge.time, threshold);
            bool misoBit = miso.isEmpty() ? false : sampleLevel(miso, edge.time, threshold);
            
            if (m_spiMSBFirst) {
                mosiWord = (mosiWord << 1) | (mosiBit ? 1 : 0);
                misoWord = (misoWord << 1) | (misoBit ? 1 : 0);
            } else {
                mosiWord |= (mosiBit ? 1 : 0) << bitCount;
                misoWord |= (misoBit ? 1 : 0) << bitCount;
            }
            
            bitCount++;
            
            if (bitCount >= m_spiWordSize) {
                DecodedFrame frame;
                frame.startTime = wordStart;
                frame.endTime = edge.time;
                frame.type = "DATA";
                frame.description = QString("MOSI: 0x%1  MISO: 0x%2")
                                   .arg(mosiWord, m_spiWordSize/4, 16, QChar('0'))
                                   .arg(misoWord, m_spiWordSize/4, 16, QChar('0'));
                frames.append(frame);
                
                bitCount = 0;
                mosiWord = 0;
                misoWord = 0;
            }
        }
    }
    
    return frames;
}

QVector<ProtocolDecoder::Edge> ProtocolDecoder::findEdges(
    const QVector<QPointF> &data, 
    double threshold)
{
    QVector<Edge> edges;
    
    if (data.size() < 2) {
        return edges;
    }
    
    bool prevLevel = data[0].y() > threshold;
    
    for (int i = 1; i < data.size(); ++i) {
        bool currLevel = data[i].y() > threshold;
        
        if (currLevel != prevLevel) {
            Edge edge;
            // Interpolate exact crossing time
            double t0 = data[i-1].x();
            double t1 = data[i].x();
            double v0 = data[i-1].y();
            double v1 = data[i].y();
            
            edge.time = t0 + (threshold - v0) * (t1 - t0) / (v1 - v0);
            edge.rising = currLevel;
            edges.append(edge);
        }
        
        prevLevel = currLevel;
    }
    
    return edges;
}

bool ProtocolDecoder::sampleLevel(
    const QVector<QPointF> &data, 
    double time, 
    double threshold)
{
    // Binary search for the sample closest to time
    int left = 0;
    int right = data.size() - 1;
    
    while (left < right) {
        int mid = (left + right) / 2;
        if (data[mid].x() < time) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    
    // Interpolate if possible
    if (left > 0 && left < data.size()) {
        double t0 = data[left-1].x();
        double t1 = data[left].x();
        double v0 = data[left-1].y();
        double v1 = data[left].y();
        
        double v = v0 + (time - t0) * (v1 - v0) / (t1 - t0);
        return v > threshold;
    }
    
    return data[left].y() > threshold;
}

double ProtocolDecoder::estimateBitPeriod(const QVector<Edge> &edges)
{
    if (edges.size() < 4) {
        return 0.0;
    }
    
    // Calculate period between consecutive same-direction edges
    QVector<double> periods;
    
    for (int i = 2; i < edges.size(); ++i) {
        if (edges[i].rising == edges[i-2].rising) {
            periods.append(edges[i].time - edges[i-2].time);
        }
    }
    
    if (periods.isEmpty()) {
        return 0.0;
    }
    
    // Return median
    std::sort(periods.begin(), periods.end());
    return periods[periods.size() / 2] / 2.0;  // Divide by 2 because we measured 2 bit periods
}
