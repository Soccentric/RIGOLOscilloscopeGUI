#include "waveformprovider.h"
#include "deviceconnection.h"
#include "scpicommands.h"

WaveformProvider::WaveformProvider(DeviceConnection *connection, QObject *parent)
    : QObject(parent)
    , m_connection(connection)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &WaveformProvider::onAcquisitionTimer);
}

void WaveformProvider::startContinuousAcquisition(int intervalMs)
{
    if (m_activeChannels.isEmpty()) {
        emit acquisitionError("No channels configured for acquisition");
        return;
    }
    
    m_timer->setInterval(intervalMs);
    m_timer->start();
    m_currentChannelIndex = 0;
    emit acquisitionStarted();
}

void WaveformProvider::stopContinuousAcquisition()
{
    m_timer->stop();
    emit acquisitionStopped();
}

void WaveformProvider::acquireWaveform(const QString &channel)
{
    if (!m_connection->isConnected()) {
        emit acquisitionError("Not connected to device");
        return;
    }
    
    // Set source
    m_connection->sendCommand(SCPI::waveformSource(channel));
    
    // Set format to BYTE for efficiency
    m_connection->sendCommand(SCPI::waveformFormat("BYTE"));
    
    // Set mode
    QString modeStr;
    switch (m_mode) {
        case WaveformMode::Normal: modeStr = "NORMal"; break;
        case WaveformMode::Maximum: modeStr = "MAXimum"; break;
        case WaveformMode::Raw: modeStr = "RAW"; break;
    }
    m_connection->sendCommand(SCPI::waveformMode(modeStr));
    
    // Set points range
    m_connection->sendCommand(SCPI::waveformStart(m_startPoint));
    m_connection->sendCommand(SCPI::waveformStop(m_stopPoint));
    
    // Request preamble
    requestPreamble(channel);
    
    // Request data
    requestWaveformData(channel);
}

void WaveformProvider::acquireAllChannels(const QStringList &channels)
{
    m_activeChannels = channels;
    m_currentChannelIndex = 0;
    
    for (const QString &channel : channels) {
        acquireWaveform(channel);
    }
}

void WaveformProvider::setWaveformMode(WaveformMode mode)
{
    m_mode = mode;
}

WaveformProvider::WaveformPreamble WaveformProvider::preamble(const QString &channel) const
{
    return m_preambles.value(channel);
}

void WaveformProvider::onAcquisitionTimer()
{
    if (m_activeChannels.isEmpty()) return;
    
    // Acquire one channel at a time in round-robin fashion
    QString channel = m_activeChannels[m_currentChannelIndex];
    acquireWaveform(channel);
    
    m_currentChannelIndex = (m_currentChannelIndex + 1) % m_activeChannels.size();
}

void WaveformProvider::requestPreamble(const QString &channel)
{
    QString response = m_connection->sendQuery(SCPI::WAVEFORM_PREAMBLE_QUERY, 2000);
    if (!response.isEmpty()) {
        WaveformPreamble preamble = parsePreamble(response);
        m_preambles[channel] = preamble;
        emit preambleUpdated(channel, preamble);
    }
}

void WaveformProvider::requestWaveformData(const QString &channel)
{
    m_connection->sendCommand(QString(SCPI::WAVEFORM_DATA_QUERY));
    
    // Read binary response
    QByteArray data = m_connection->readBinaryData(-1, 5000);
    
    if (!data.isEmpty()) {
        WaveformPreamble preamble = m_preambles.value(channel);
        QVector<QPointF> points = parseWaveformData(data, preamble);
        
        if (!points.isEmpty()) {
            emit waveformDataReady(channel, points);
        }
    }
}

WaveformProvider::WaveformPreamble WaveformProvider::parsePreamble(const QString &response)
{
    WaveformPreamble preamble;
    QStringList parts = response.split(',');
    
    if (parts.size() >= 10) {
        preamble.format = parts[0].toInt();
        preamble.type = parts[1].toInt();
        preamble.points = parts[2].toInt();
        preamble.count = parts[3].toInt();
        preamble.xIncrement = parts[4].toDouble();
        preamble.xOrigin = parts[5].toDouble();
        preamble.xReference = parts[6].toDouble();
        preamble.yIncrement = parts[7].toDouble();
        preamble.yOrigin = parts[8].toDouble();
        preamble.yReference = parts[9].toDouble();
    }
    
    return preamble;
}

QVector<QPointF> WaveformProvider::parseWaveformData(const QByteArray &data, const WaveformPreamble &preamble)
{
    QVector<QPointF> points;
    points.reserve(data.size());
    
    for (int i = 0; i < data.size(); ++i) {
        // For BYTE format, data is unsigned 8-bit
        unsigned char raw = static_cast<unsigned char>(data[i]);
        
        // Apply scaling
        double voltage = (raw - preamble.yReference - preamble.yOrigin) * preamble.yIncrement;
        double time = (i - preamble.xReference) * preamble.xIncrement + preamble.xOrigin;
        
        points.append(QPointF(time, voltage));
    }
    
    return points;
}
