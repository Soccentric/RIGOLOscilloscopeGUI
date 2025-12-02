#include "dataexporter.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <cmath>

DataExporter::DataExporter(QObject *parent)
    : QObject(parent)
{
}

bool DataExporter::exportWaveforms(
    const QString &filename,
    const QVector<WaveformData> &waveforms,
    Format format,
    const ExportOptions &options)
{
    if (waveforms.isEmpty()) {
        m_lastError = "No waveform data to export";
        emit exportError(m_lastError);
        return false;
    }

    bool success = false;

    switch (format) {
        case Format::CSV:
        case Format::TSV:
            success = exportToCSV(filename, waveforms, options);
            break;
        case Format::Binary:
            success = exportToBinary(filename, waveforms, options);
            break;
        case Format::MATLAB:
            success = exportToMATLAB(filename, waveforms, options);
            break;
        case Format::VCD:
            success = exportToVCD(filename, waveforms, options);
            break;
        case Format::WAV:
            success = exportToWAV(filename, waveforms, options);
            break;
        case Format::JSON:
            success = exportToJSON(filename, waveforms, options);
            break;
        default:
            m_lastError = "Unsupported format";
            emit exportError(m_lastError);
            return false;
    }

    if (success) {
        emit exportComplete(filename);
    }

    return success;
}

bool DataExporter::exportMeasurements(
    const QString &filename,
    const QVector<MeasurementData> &measurements,
    Format format,
    const ExportOptions &options)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = QString("Cannot open file: %1").arg(file.errorString());
        emit exportError(m_lastError);
        return false;
    }

    QTextStream out(&file);
    QString delim = (format == Format::TSV) ? "\t" : options.delimiter;

    // Header
    if (options.includeHeader) {
        if (options.includeTimestamp) {
            out << "Timestamp" << delim;
        }
        out << "Channel" << delim << "Measurement" << delim 
            << "Value" << delim << "Unit\n";
    }

    QString timestamp = QDateTime::currentDateTime().toString(options.dateFormat);

    for (const auto &m : measurements) {
        if (options.includeTimestamp) {
            out << timestamp << delim;
        }
        out << m.channel << delim << m.type << delim 
            << formatNumber(m.value, options) << delim << m.unit << "\n";
    }

    file.close();
    emit exportComplete(filename);
    return true;
}

bool DataExporter::exportFFT(
    const QString &filename,
    const QVector<double> &frequencies,
    const QVector<double> &magnitudes,
    Format format,
    const ExportOptions &options)
{
    if (frequencies.size() != magnitudes.size()) {
        m_lastError = "Frequency and magnitude arrays must have same size";
        emit exportError(m_lastError);
        return false;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = QString("Cannot open file: %1").arg(file.errorString());
        emit exportError(m_lastError);
        return false;
    }

    QTextStream out(&file);
    QString delim = (format == Format::TSV) ? "\t" : options.delimiter;

    if (options.includeHeader) {
        if (options.includeTimestamp) {
            out << "# Exported: " << QDateTime::currentDateTime().toString(options.dateFormat) << "\n";
        }
        out << "Frequency (Hz)" << delim << "Magnitude (dB)\n";
    }

    int total = frequencies.size();
    for (int i = 0; i < total; ++i) {
        out << formatNumber(frequencies[i], options) << delim 
            << formatNumber(magnitudes[i], options) << "\n";
        
        if (i % 1000 == 0) {
            emit exportProgress(100 * i / total);
        }
    }

    file.close();
    emit exportComplete(filename);
    return true;
}

bool DataExporter::exportProtocolData(
    const QString &filename,
    const QVector<QPair<double, QString>> &events,
    Format format,
    const ExportOptions &options)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = QString("Cannot open file: %1").arg(file.errorString());
        emit exportError(m_lastError);
        return false;
    }

    QTextStream out(&file);

    if (format == Format::VCD) {
        // VCD header
        out << "$date\n   " << QDateTime::currentDateTime().toString() << "\n$end\n";
        out << "$version\n   RIGOL Oscilloscope GUI\n$end\n";
        out << "$timescale 1 ns $end\n";
        out << "$scope module top $end\n";
        out << "$var wire 8 ! data $end\n";
        out << "$upscope $end\n";
        out << "$enddefinitions $end\n";
        out << "#0\n";

        for (const auto &event : events) {
            long long timeNs = static_cast<long long>(event.first * 1e9);
            out << "#" << timeNs << "\n";
            out << "b" << event.second << " !\n";
        }
    } else {
        // CSV format
        QString delim = options.delimiter;
        if (options.includeHeader) {
            out << "Time (s)" << delim << "Event\n";
        }
        for (const auto &event : events) {
            out << formatNumber(event.first, options) << delim << event.second << "\n";
        }
    }

    file.close();
    emit exportComplete(filename);
    return true;
}

bool DataExporter::exportToCSV(
    const QString &filename,
    const QVector<WaveformData> &waveforms,
    const ExportOptions &options)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = QString("Cannot open file: %1").arg(file.errorString());
        emit exportError(m_lastError);
        return false;
    }

    QTextStream out(&file);
    QString delim = options.delimiter;

    // Header with metadata
    if (options.includeHeader) {
        if (options.includeTimestamp) {
            out << "# Exported: " << QDateTime::currentDateTime().toString(options.dateFormat) << "\n";
        }
        out << "Time (s)";
        for (const auto &wf : waveforms) {
            out << delim << wf.name << " (" << wf.unit << ")";
        }
        out << "\n";
    }

    // Find maximum sample count
    int maxSamples = 0;
    for (const auto &wf : waveforms) {
        maxSamples = qMax(maxSamples, wf.data.size());
    }

    // Export data
    for (int i = 0; i < maxSamples; ++i) {
        // Time from first channel (assuming all channels have same timebase)
        double time = waveforms[0].data.size() > i ? waveforms[0].data[i].x() : 0.0;
        out << formatNumber(time, options);

        for (const auto &wf : waveforms) {
            out << delim;
            if (i < wf.data.size()) {
                out << formatNumber(wf.data[i].y(), options);
            }
        }
        out << "\n";

        if (i % 10000 == 0) {
            emit exportProgress(100 * i / maxSamples);
        }
    }

    file.close();
    return true;
}

bool DataExporter::exportToBinary(
    const QString &filename,
    const QVector<WaveformData> &waveforms,
    const ExportOptions &options)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        m_lastError = QString("Cannot open file: %1").arg(file.errorString());
        emit exportError(m_lastError);
        return false;
    }

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setFloatingPointPrecision(QDataStream::DoublePrecision);

    // Header
    out << QString("RIGOL_WFM");  // Magic
    out << static_cast<quint32>(1);  // Version
    out << static_cast<quint32>(waveforms.size());  // Channel count
    out << options.sampleRate;  // Sample rate

    // Channel info
    for (const auto &wf : waveforms) {
        out << wf.name;
        out << static_cast<quint32>(wf.data.size());
        out << wf.voltageScale;
        out << wf.voltageOffset;
        out << wf.timeScale;
    }

    // Waveform data
    for (const auto &wf : waveforms) {
        for (const auto &pt : wf.data) {
            out << pt.x() << pt.y();
        }
    }

    file.close();
    return true;
}

bool DataExporter::exportToMATLAB(
    const QString &filename,
    const QVector<WaveformData> &waveforms,
    const ExportOptions &options)
{
    // Simple MAT-file format (Level 4)
    // For full MAT-file support, use matio library
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        m_lastError = QString("Cannot open file: %1").arg(file.errorString());
        emit exportError(m_lastError);
        return false;
    }

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);

    for (const auto &wf : waveforms) {
        // MAT v4 header
        // Type: 0000 = double
        quint32 type = 0;  // IEEE double precision
        quint32 mrows = wf.data.size();
        quint32 ncols = 2;  // Time and value
        quint32 imagf = 0;  // No imaginary part
        quint32 namelen = wf.name.length() + 1;

        out << type << mrows << ncols << imagf << namelen;
        
        // Variable name
        QByteArray name = wf.name.toLatin1();
        name.append('\0');
        file.write(name);

        // Data (column-major order)
        // Time column
        for (const auto &pt : wf.data) {
            double t = pt.x();
            out.writeRawData(reinterpret_cast<const char*>(&t), sizeof(double));
        }
        // Value column
        for (const auto &pt : wf.data) {
            double v = pt.y();
            out.writeRawData(reinterpret_cast<const char*>(&v), sizeof(double));
        }
    }

    file.close();
    return true;
}

bool DataExporter::exportToVCD(
    const QString &filename,
    const QVector<WaveformData> &waveforms,
    const ExportOptions &options)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = QString("Cannot open file: %1").arg(file.errorString());
        emit exportError(m_lastError);
        return false;
    }

    QTextStream out(&file);

    // VCD header
    out << "$date\n   " << QDateTime::currentDateTime().toString() << "\n$end\n";
    out << "$version\n   RIGOL Oscilloscope GUI\n$end\n";
    out << "$timescale 1 ps $end\n";  // Picoseconds for high resolution
    out << "$scope module oscilloscope $end\n";

    // Define variables
    QVector<char> varIds;
    char varId = '!';
    for (const auto &wf : waveforms) {
        out << "$var wire 1 " << varId << " " << wf.name << " $end\n";
        varIds.append(varId);
        varId++;
    }

    out << "$upscope $end\n";
    out << "$enddefinitions $end\n";
    out << "$dumpvars\n";

    // Initial values
    for (int i = 0; i < waveforms.size(); ++i) {
        if (!waveforms[i].data.isEmpty()) {
            bool level = waveforms[i].data[0].y() > 0.5;
            out << (level ? "1" : "0") << varIds[i] << "\n";
        }
    }
    out << "$end\n";

    // Find all edge times and sort
    QVector<QPair<long long, QPair<int, bool>>> edges;
    
    for (int ch = 0; ch < waveforms.size(); ++ch) {
        const auto &data = waveforms[ch].data;
        for (int i = 1; i < data.size(); ++i) {
            bool prev = data[i-1].y() > 0.5;
            bool curr = data[i].y() > 0.5;
            if (prev != curr) {
                long long timePs = static_cast<long long>(data[i].x() * 1e12);
                edges.append({timePs, {ch, curr}});
            }
        }
    }

    std::sort(edges.begin(), edges.end());

    long long lastTime = -1;
    for (const auto &edge : edges) {
        if (edge.first != lastTime) {
            out << "#" << edge.first << "\n";
            lastTime = edge.first;
        }
        out << (edge.second.second ? "1" : "0") << varIds[edge.second.first] << "\n";
    }

    file.close();
    return true;
}

bool DataExporter::exportToWAV(
    const QString &filename,
    const QVector<WaveformData> &waveforms,
    const ExportOptions &options)
{
    if (waveforms.isEmpty()) {
        m_lastError = "No waveform data";
        emit exportError(m_lastError);
        return false;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        m_lastError = QString("Cannot open file: %1").arg(file.errorString());
        emit exportError(m_lastError);
        return false;
    }

    const WaveformData &wf = waveforms[0];  // Use first channel
    int numSamples = wf.data.size();
    int sampleRate = static_cast<int>(options.sampleRate);
    int numChannels = qMin(waveforms.size(), 2);  // Max 2 channels for WAV
    int bitsPerSample = 16;
    int byteRate = sampleRate * numChannels * bitsPerSample / 8;
    int blockAlign = numChannels * bitsPerSample / 8;
    int dataSize = numSamples * blockAlign;

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);

    // RIFF header
    file.write("RIFF");
    out << static_cast<quint32>(36 + dataSize);
    file.write("WAVE");

    // fmt chunk
    file.write("fmt ");
    out << static_cast<quint32>(16);  // Chunk size
    out << static_cast<quint16>(1);   // Audio format (PCM)
    out << static_cast<quint16>(numChannels);
    out << static_cast<quint32>(sampleRate);
    out << static_cast<quint32>(byteRate);
    out << static_cast<quint16>(blockAlign);
    out << static_cast<quint16>(bitsPerSample);

    // data chunk
    file.write("data");
    out << static_cast<quint32>(dataSize);

    // Normalize and convert samples
    double maxVal = 0.0;
    for (int ch = 0; ch < numChannels; ++ch) {
        for (const auto &pt : waveforms[ch].data) {
            maxVal = qMax(maxVal, std::abs(pt.y()));
        }
    }
    if (maxVal == 0.0) maxVal = 1.0;

    for (int i = 0; i < numSamples; ++i) {
        for (int ch = 0; ch < numChannels; ++ch) {
            double val = 0.0;
            if (i < waveforms[ch].data.size()) {
                val = waveforms[ch].data[i].y() / maxVal;
            }
            qint16 sample = static_cast<qint16>(val * 32767.0);
            out << sample;
        }
        
        if (i % 10000 == 0) {
            emit exportProgress(100 * i / numSamples);
        }
    }

    file.close();
    return true;
}

bool DataExporter::exportToJSON(
    const QString &filename,
    const QVector<WaveformData> &waveforms,
    const ExportOptions &options)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = QString("Cannot open file: %1").arg(file.errorString());
        emit exportError(m_lastError);
        return false;
    }

    QJsonObject root;
    
    // Metadata
    QJsonObject meta;
    meta["exported"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    meta["generator"] = "RIGOL Oscilloscope GUI";
    meta["sampleRate"] = options.sampleRate;
    root["metadata"] = meta;

    // Channels
    QJsonArray channels;
    for (const auto &wf : waveforms) {
        QJsonObject channel;
        channel["name"] = wf.name;
        channel["unit"] = wf.unit;
        channel["voltageScale"] = wf.voltageScale;
        channel["voltageOffset"] = wf.voltageOffset;
        channel["timeScale"] = wf.timeScale;
        channel["sampleCount"] = wf.data.size();

        QJsonArray time;
        QJsonArray values;
        for (const auto &pt : wf.data) {
            time.append(pt.x());
            values.append(pt.y());
        }
        channel["time"] = time;
        channel["values"] = values;

        channels.append(channel);
    }
    root["channels"] = channels;

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

QString DataExporter::formatNumber(double value, const ExportOptions &options)
{
    if (std::isnan(value)) return "NaN";
    if (std::isinf(value)) return value > 0 ? "Inf" : "-Inf";

    if (options.scientificNotation) {
        return QString::number(value, 'e', options.precision);
    } else {
        return QString::number(value, 'g', options.precision);
    }
}
