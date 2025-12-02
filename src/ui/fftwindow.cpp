#include "fftwindow.h"
#include "../core/scopechannel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QtMath>
#include <QDebug>

FFTWindow::FFTWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_fftAnalyzer(new FFTAnalyzer(this))
{
    setWindowTitle(tr("FFT Spectrum Analysis"));
    setMinimumSize(1000, 700);
    resize(1200, 800);

    setupUI();
    setupChart();

    // Update timer for real-time analysis
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &FFTWindow::onUpdateTimer);
    m_updateTimer->start(500); // Update every 500ms

    connect(m_fftAnalyzer, &FFTAnalyzer::parametersChanged, this, &FFTWindow::updateChart);
}

void FFTWindow::setSourceChannel(ScopeChannel *channel)
{
    m_sourceChannel = channel;
    if (channel) {
        setWindowTitle(tr("FFT Spectrum Analysis - %1").arg(channel->name()));
        updateData();
    }
}

void FFTWindow::updateData()
{
    if (!m_sourceChannel || m_sourceChannel->data().isEmpty()) {
        return;
    }

    // Extract voltage data
    QVector<double> voltageData;
    voltageData.reserve(m_sourceChannel->data().size());
    for (const QPointF &point : m_sourceChannel->data()) {
        voltageData.append(point.y() / m_sourceChannel->probeFactor());
    }

    // Assume sample rate (this should come from the device)
    double sampleRate = 1000000.0; // 1 MHz default

    // Compute FFT
    m_magnitudes = m_fftAnalyzer->compute(voltageData, sampleRate);
    m_frequencies = m_fftAnalyzer->frequencyAxis(sampleRate);

    // Find peaks if enabled
    if (m_peakDetectCheck->isChecked()) {
        m_peaks = m_fftAnalyzer->findPeaks(m_magnitudes, sampleRate,
                                          m_maxPeaksSpin->value(),
                                          m_peakThresholdSpin->value());
    } else {
        m_peaks.clear();
    }

    updateChart();
    updateMeasurements();
}

void FFTWindow::setupUI()
{
    // Create central widget
    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Chart area
    m_chartView = new QChartView;
    m_chartView->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(m_chartView, 1);

    // Control panel
    QWidget *controlPanel = new QWidget;
    controlPanel->setFixedWidth(300);
    controlPanel->setStyleSheet("QWidget { background-color: #f5f5f5; }");
    mainLayout->addWidget(controlPanel);

    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);

    // Source selection
    QGroupBox *sourceGroup = new QGroupBox(tr("Source"));
    QVBoxLayout *sourceLayout = new QVBoxLayout(sourceGroup);
    m_sourceCombo = new QComboBox;
    m_sourceCombo->addItem(tr("CH1"));
    m_sourceCombo->addItem(tr("CH2"));
    m_sourceCombo->addItem(tr("CH3"));
    m_sourceCombo->addItem(tr("CH4"));
    sourceLayout->addWidget(new QLabel(tr("Channel:")));
    sourceLayout->addWidget(m_sourceCombo);
    controlLayout->addWidget(sourceGroup);

    // FFT Parameters
    QGroupBox *fftGroup = new QGroupBox(tr("FFT Parameters"));
    QGridLayout *fftLayout = new QGridLayout(fftGroup);

    // Window function
    fftLayout->addWidget(new QLabel(tr("Window:")), 0, 0);
    m_windowCombo = new QComboBox;
    m_windowCombo->addItem(tr("Rectangle"), static_cast<int>(FFTAnalyzer::WindowFunction::Rectangle));
    m_windowCombo->addItem(tr("Hanning"), static_cast<int>(FFTAnalyzer::WindowFunction::Hanning));
    m_windowCombo->addItem(tr("Hamming"), static_cast<int>(FFTAnalyzer::WindowFunction::Hamming));
    m_windowCombo->addItem(tr("Blackman"), static_cast<int>(FFTAnalyzer::WindowFunction::Blackman));
    m_windowCombo->addItem(tr("Flat Top"), static_cast<int>(FFTAnalyzer::WindowFunction::FlatTop));
    m_windowCombo->addItem(tr("Kaiser"), static_cast<int>(FFTAnalyzer::WindowFunction::Kaiser));
    m_windowCombo->setCurrentIndex(1); // Hanning
    connect(m_windowCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FFTWindow::onWindowFunctionChanged);
    fftLayout->addWidget(m_windowCombo, 0, 1);

    // Scale type
    fftLayout->addWidget(new QLabel(tr("Scale:")), 1, 0);
    m_scaleCombo = new QComboBox;
    m_scaleCombo->addItem(tr("Linear"), static_cast<int>(FFTAnalyzer::ScaleType::Linear));
    m_scaleCombo->addItem(tr("Log (dB)"), static_cast<int>(FFTAnalyzer::ScaleType::Logarithmic));
    m_scaleCombo->addItem(tr("dBV"), static_cast<int>(FFTAnalyzer::ScaleType::dBV));
    m_scaleCombo->addItem(tr("dBm"), static_cast<int>(FFTAnalyzer::ScaleType::dBm));
    m_scaleCombo->setCurrentIndex(1); // Log
    connect(m_scaleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FFTWindow::onScaleTypeChanged);
    fftLayout->addWidget(m_scaleCombo, 1, 1);

    // FFT Size
    fftLayout->addWidget(new QLabel(tr("FFT Size:")), 2, 0);
    m_fftSizeSpin = new QSpinBox;
    m_fftSizeSpin->setRange(256, 65536);
    m_fftSizeSpin->setValue(4096);
    m_fftSizeSpin->setSingleStep(256);
    connect(m_fftSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FFTWindow::onFFTSizeChanged);
    fftLayout->addWidget(m_fftSizeSpin, 2, 1);

    // Overlap
    fftLayout->addWidget(new QLabel(tr("Overlap:")), 3, 0);
    m_overlapSpin = new QDoubleSpinBox;
    m_overlapSpin->setRange(0.0, 0.9);
    m_overlapSpin->setValue(0.5);
    m_overlapSpin->setSingleStep(0.1);
    m_overlapSpin->setSuffix(" %");
    connect(m_overlapSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FFTWindow::onOverlapChanged);
    fftLayout->addWidget(m_overlapSpin, 3, 1);

    // Averaging
    fftLayout->addWidget(new QLabel(tr("Averaging:")), 4, 0);
    m_avgCountSpin = new QSpinBox;
    m_avgCountSpin->setRange(1, 100);
    m_avgCountSpin->setValue(1);
    connect(m_avgCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FFTWindow::onAveragingChanged);
    fftLayout->addWidget(m_avgCountSpin, 4, 1);

    // Reference level
    fftLayout->addWidget(new QLabel(tr("Ref Level:")), 5, 0);
    m_refLevelSpin = new QDoubleSpinBox;
    m_refLevelSpin->setRange(0.001, 100.0);
    m_refLevelSpin->setValue(1.0);
    m_refLevelSpin->setSuffix(" V");
    connect(m_refLevelSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FFTWindow::onReferenceLevelChanged);
    fftLayout->addWidget(m_refLevelSpin, 5, 1);

    controlLayout->addWidget(fftGroup);

    // Peak Detection
    QGroupBox *peakGroup = new QGroupBox(tr("Peak Detection"));
    QGridLayout *peakLayout = new QGridLayout(peakGroup);

    m_peakDetectCheck = new QCheckBox(tr("Enable"));
    m_peakDetectCheck->setChecked(true);
    connect(m_peakDetectCheck, &QCheckBox::toggled, this, &FFTWindow::onPeakDetectionToggled);
    peakLayout->addWidget(m_peakDetectCheck, 0, 0, 1, 2);

    peakLayout->addWidget(new QLabel(tr("Max Peaks:")), 1, 0);
    m_maxPeaksSpin = new QSpinBox;
    m_maxPeaksSpin->setRange(1, 50);
    m_maxPeaksSpin->setValue(10);
    peakLayout->addWidget(m_maxPeaksSpin, 1, 1);

    peakLayout->addWidget(new QLabel(tr("Threshold:")), 2, 0);
    m_peakThresholdSpin = new QDoubleSpinBox;
    m_peakThresholdSpin->setRange(-200.0, 0.0);
    m_peakThresholdSpin->setValue(-60.0);
    m_peakThresholdSpin->setSuffix(" dB");
    peakLayout->addWidget(m_peakThresholdSpin, 2, 1);

    controlLayout->addWidget(peakGroup);

    // Measurements
    QGroupBox *measGroup = new QGroupBox(tr("Measurements"));
    QVBoxLayout *measLayout = new QVBoxLayout(measGroup);

    m_fundamentalLabel = new QLabel(tr("Fundamental: ---"));
    m_thdLabel = new QLabel(tr("THD: ---"));
    m_sfdrLabel = new QLabel(tr("SFDR: ---"));
    m_snrLabel = new QLabel(tr("SNR: ---"));

    measLayout->addWidget(m_fundamentalLabel);
    measLayout->addWidget(m_thdLabel);
    measLayout->addWidget(m_sfdrLabel);
    measLayout->addWidget(m_snrLabel);

    controlLayout->addWidget(measGroup);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    QPushButton *autoScaleBtn = new QPushButton(tr("Auto Scale"));
    connect(autoScaleBtn, &QPushButton::clicked, this, &FFTWindow::onAutoScale);
    buttonLayout->addWidget(autoScaleBtn);

    QPushButton *exportBtn = new QPushButton(tr("Export"));
    connect(exportBtn, &QPushButton::clicked, this, &FFTWindow::onExportData);
    buttonLayout->addWidget(exportBtn);

    controlLayout->addLayout(buttonLayout);

    // Spacer
    controlLayout->addStretch();
}

void FFTWindow::setupChart()
{
    m_chart = new QChart;
    m_chart->setTitle(tr("FFT Spectrum"));
    m_chart->setAnimationOptions(QChart::NoAnimation);

    m_spectrumSeries = new QLineSeries;
    m_spectrumSeries->setName(tr("Spectrum"));
    m_chart->addSeries(m_spectrumSeries);

    m_xAxis = new QValueAxis;
    m_xAxis->setTitleText(tr("Frequency (Hz)"));
    m_xAxis->setLabelFormat("%.1e");
    m_chart->addAxis(m_xAxis, Qt::AlignBottom);
    m_spectrumSeries->attachAxis(m_xAxis);

    m_yAxis = new QValueAxis;
    m_yAxis->setTitleText(tr("Magnitude (dB)"));
    m_yAxis->setLabelFormat("%.1f");
    m_chart->addAxis(m_yAxis, Qt::AlignLeft);
    m_spectrumSeries->attachAxis(m_yAxis);

    m_chartView->setChart(m_chart);
}

void FFTWindow::updateChart()
{
    if (m_frequencies.isEmpty() || m_magnitudes.isEmpty()) {
        return;
    }

    m_spectrumSeries->clear();

    // Add data points
    for (int i = 0; i < m_frequencies.size() && i < m_magnitudes.size(); ++i) {
        if (m_frequencies[i] >= 0) { // Only positive frequencies
            m_spectrumSeries->append(m_frequencies[i], m_magnitudes[i]);
        }
    }

    // Update axes
    double maxFreq = 0;
    double minMag = 0, maxMag = -200;

    for (int i = 0; i < m_frequencies.size() && i < m_magnitudes.size(); ++i) {
        if (m_frequencies[i] > maxFreq) maxFreq = m_frequencies[i];
        if (m_magnitudes[i] > maxMag) maxMag = m_magnitudes[i];
        if (m_magnitudes[i] < minMag) minMag = m_magnitudes[i];
    }

    m_xAxis->setRange(0, maxFreq);
    m_yAxis->setRange(minMag - 10, maxMag + 10);

    updatePeakMarkers();
}

void FFTWindow::updatePeakMarkers()
{
    // Clear existing peak markers
    for (QLineSeries *series : m_peakMarkers) {
        m_chart->removeSeries(series);
        delete series;
    }
    m_peakMarkers.clear();

    // Add new peak markers
    for (const FFTAnalyzer::Peak &peak : m_peaks) {
        QLineSeries *marker = new QLineSeries;
        marker->setName(QString("Peak %1").arg(m_peakMarkers.size() + 1));
        marker->setColor(Qt::red);
        marker->append(peak.frequency, m_yAxis->min());
        marker->append(peak.frequency, peak.magnitude);
        m_chart->addSeries(marker);
        marker->attachAxis(m_xAxis);
        marker->attachAxis(m_yAxis);
        m_peakMarkers.append(marker);
    }
}

void FFTWindow::updateMeasurements()
{
    if (m_peaks.isEmpty()) {
        m_fundamentalLabel->setText(tr("Fundamental: ---"));
        m_thdLabel->setText(tr("THD: ---"));
        m_sfdrLabel->setText(tr("SFDR: ---"));
        m_snrLabel->setText(tr("SNR: ---"));
        return;
    }

    // Assume first peak is fundamental
    const FFTAnalyzer::Peak &fundamental = m_peaks.first();
    m_fundamentalLabel->setText(tr("Fundamental: %1 @ %2")
                               .arg(formatMagnitude(fundamental.magnitude))
                               .arg(formatFrequency(fundamental.frequency)));

    // Calculate THD (simplified - assumes fundamental is first peak)
    double sampleRate = 1000000.0; // Should get from device
    double thd = m_fftAnalyzer->calculateTHD(m_magnitudes, fundamental.frequency, sampleRate);
    m_thdLabel->setText(tr("THD: %.2f%%").arg(thd * 100));

    // Calculate SFDR
    double sfdr = m_fftAnalyzer->calculateSFDR(m_magnitudes, fundamental.frequency, sampleRate);
    m_sfdrLabel->setText(tr("SFDR: %.1f dB").arg(sfdr));

    // Calculate SNR
    double snr = m_fftAnalyzer->calculateSNR(m_magnitudes, fundamental.frequency, sampleRate);
    m_snrLabel->setText(tr("SNR: %.1f dB").arg(snr));
}

QString FFTWindow::formatFrequency(double hz) const
{
    if (hz >= 1e6) {
        return QString("%1 MHz").arg(hz / 1e6, 0, 'f', 2);
    } else if (hz >= 1e3) {
        return QString("%1 kHz").arg(hz / 1e3, 0, 'f', 2);
    } else {
        return QString("%1 Hz").arg(hz, 0, 'f', 2);
    }
}

QString FFTWindow::formatMagnitude(double mag) const
{
    return QString("%1 dB").arg(mag, 0, 'f', 1);
}

void FFTWindow::onWindowFunctionChanged(int index)
{
    FFTAnalyzer::WindowFunction window = static_cast<FFTAnalyzer::WindowFunction>(
        m_windowCombo->itemData(index).toInt());
    m_fftAnalyzer->setWindowFunction(window);
}

void FFTWindow::onScaleTypeChanged(int index)
{
    FFTAnalyzer::ScaleType scale = static_cast<FFTAnalyzer::ScaleType>(
        m_scaleCombo->itemData(index).toInt());
    m_fftAnalyzer->setScaleType(scale);
    m_yAxis->setTitleText(scale == FFTAnalyzer::ScaleType::Linear ? tr("Magnitude") : tr("Magnitude (dB)"));
}

void FFTWindow::onFFTSizeChanged(int value)
{
    m_fftAnalyzer->setFFTSize(value);
}

void FFTWindow::onOverlapChanged(double value)
{
    m_fftAnalyzer->setOverlap(value);
}

void FFTWindow::onAveragingChanged(int value)
{
    m_fftAnalyzer->setAveraging(value);
}

void FFTWindow::onReferenceLevelChanged(double value)
{
    m_fftAnalyzer->setReferenceLevel(value);
}

void FFTWindow::onPeakDetectionToggled(bool enabled)
{
    if (enabled) {
        updateData();
    } else {
        m_peaks.clear();
        updatePeakMarkers();
    }
}

void FFTWindow::onExportData()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Export FFT Data"),
        QString(), tr("CSV Files (*.csv);;Text Files (*.txt)"));

    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Export Failed"),
            tr("Could not open file for writing."));
        return;
    }

    QTextStream out(&file);
    out << "Frequency (Hz),Magnitude (dB)\n";

    for (int i = 0; i < m_frequencies.size() && i < m_magnitudes.size(); ++i) {
        if (m_frequencies[i] >= 0) {
            out << m_frequencies[i] << "," << m_magnitudes[i] << "\n";
        }
    }

    file.close();
    QMessageBox::information(this, tr("Export Complete"),
        tr("FFT data exported successfully."));
}

void FFTWindow::onAutoScale()
{
    if (m_magnitudes.isEmpty()) {
        return;
    }

    double minMag = *std::min_element(m_magnitudes.begin(), m_magnitudes.end());
    double maxMag = *std::max_element(m_magnitudes.begin(), m_magnitudes.end());

    m_yAxis->setRange(minMag - 5, maxMag + 5);
}

void FFTWindow::onUpdateTimer()
{
    if (isVisible() && m_sourceChannel) {
        updateData();
    }
}