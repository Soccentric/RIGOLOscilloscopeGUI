#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QDateTime>

/**
 * @brief Data exporter for waveform and measurement data
 */
class DataExporter : public QObject
{
    Q_OBJECT

public:
    enum class Format {
        CSV,
        TSV,
        Binary,
        MATLAB,
        VCD,        // Value Change Dump (for digital signals)
        WAV,        // Audio WAV format
        PNG,        // Screenshot
        JSON
    };
    Q_ENUM(Format)

    struct ExportOptions {
        bool includeHeader;
        bool includeTimestamp;
        int precision;
        QString delimiter;
        bool scientificNotation;
        QString dateFormat;
        double sampleRate;
        
        ExportOptions() 
            : includeHeader(true)
            , includeTimestamp(true)
            , precision(6)
            , delimiter(",")
            , scientificNotation(false)
            , dateFormat("yyyy-MM-dd HH:mm:ss")
            , sampleRate(1e9)
        {}
    };

    struct WaveformData {
        QString name;
        QVector<QPointF> data;
        double voltageScale;
        double voltageOffset;
        double timeScale;
        QString unit;
        
        WaveformData()
            : voltageScale(1.0)
            , voltageOffset(0.0)
            , timeScale(1e-6)
            , unit("V")
        {}
    };

    struct MeasurementData {
        QString name;
        QString type;
        double value;
        QString unit;
        QString channel;
        
        MeasurementData() : value(0.0) {}
    };

    explicit DataExporter(QObject *parent = nullptr);
    ~DataExporter() override = default;

    // Export waveforms
    bool exportWaveforms(const QString &filename,
                         const QVector<WaveformData> &waveforms,
                         Format format = Format::CSV,
                         const ExportOptions &options = ExportOptions());

    // Export measurements
    bool exportMeasurements(const QString &filename,
                            const QVector<MeasurementData> &measurements,
                            Format format = Format::CSV,
                            const ExportOptions &options = ExportOptions());

    // Export FFT data
    bool exportFFT(const QString &filename,
                   const QVector<double> &frequencies,
                   const QVector<double> &magnitudes,
                   Format format = Format::CSV,
                   const ExportOptions &options = ExportOptions());

    // Export protocol decode results
    bool exportProtocolData(const QString &filename,
                            const QVector<QPair<double, QString>> &events,
                            Format format = Format::VCD,
                            const ExportOptions &options = ExportOptions());

    // Get last error
    QString lastError() const { return m_lastError; }

signals:
    void exportProgress(int percent);
    void exportComplete(const QString &filename);
    void exportError(const QString &error);

private:
    // CSV/TSV export
    bool exportToCSV(const QString &filename,
                     const QVector<WaveformData> &waveforms,
                     const ExportOptions &options);

    // Binary export (raw samples)
    bool exportToBinary(const QString &filename,
                        const QVector<WaveformData> &waveforms,
                        const ExportOptions &options);

    // MATLAB .mat file
    bool exportToMATLAB(const QString &filename,
                        const QVector<WaveformData> &waveforms,
                        const ExportOptions &options);

    // Value Change Dump for digital
    bool exportToVCD(const QString &filename,
                     const QVector<WaveformData> &waveforms,
                     const ExportOptions &options);

    // WAV audio format
    bool exportToWAV(const QString &filename,
                     const QVector<WaveformData> &waveforms,
                     const ExportOptions &options);

    // JSON format
    bool exportToJSON(const QString &filename,
                      const QVector<WaveformData> &waveforms,
                      const ExportOptions &options);

    // Helper: format number
    QString formatNumber(double value, const ExportOptions &options);

    QString m_lastError;
};

#endif // DATAEXPORTER_H
