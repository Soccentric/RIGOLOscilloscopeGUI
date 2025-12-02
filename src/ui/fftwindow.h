#ifndef FFTWINDOW_H
#define FFTWINDOW_H

#include <QMainWindow>
#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTimer>

#include "../analysis/fftanalyzer.h"
#include "../core/scopechannel.h"

QT_BEGIN_NAMESPACE
class QChart;
class QChartView;
QT_END_NAMESPACE

QT_USE_NAMESPACE

/**
 * @brief FFT Analysis Window for spectrum analysis
 */
class FFTWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FFTWindow(QWidget *parent = nullptr);
    ~FFTWindow() override = default;

    void setSourceChannel(ScopeChannel *channel);
    void updateData();

private slots:
    void onWindowFunctionChanged(int index);
    void onScaleTypeChanged(int index);
    void onFFTSizeChanged(int value);
    void onOverlapChanged(double value);
    void onAveragingChanged(int value);
    void onReferenceLevelChanged(double value);
    void onPeakDetectionToggled(bool enabled);
    void onExportData();
    void onAutoScale();
    void onUpdateTimer();

private:
    void setupUI();
    void setupChart();
    void updateChart();
    void updatePeakMarkers();
    void updateMeasurements();
    QString formatFrequency(double hz) const;
    QString formatMagnitude(double mag) const;

    // Core components
    FFTAnalyzer *m_fftAnalyzer;
    ScopeChannel *m_sourceChannel = nullptr;

    // Chart components
    QChart *m_chart;
    QChartView *m_chartView;
    QLineSeries *m_spectrumSeries;
    QValueAxis *m_xAxis;
    QValueAxis *m_yAxis;

    // Peak markers
    QVector<QLineSeries*> m_peakMarkers;

    // Control widgets
    QComboBox *m_sourceCombo;
    QComboBox *m_windowCombo;
    QComboBox *m_scaleCombo;
    QSpinBox *m_fftSizeSpin;
    QDoubleSpinBox *m_overlapSpin;
    QSpinBox *m_avgCountSpin;
    QDoubleSpinBox *m_refLevelSpin;
    QCheckBox *m_peakDetectCheck;
    QSpinBox *m_maxPeaksSpin;
    QDoubleSpinBox *m_peakThresholdSpin;

    // Measurement display
    QLabel *m_fundamentalLabel;
    QLabel *m_thdLabel;
    QLabel *m_sfdrLabel;
    QLabel *m_snrLabel;

    // Update timer
    QTimer *m_updateTimer;

    // Data
    QVector<double> m_frequencies;
    QVector<double> m_magnitudes;
    QVector<FFTAnalyzer::Peak> m_peaks;
};

#endif // FFTWINDOW_H