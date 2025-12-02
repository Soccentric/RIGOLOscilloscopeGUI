#ifndef FFTANALYZER_H
#define FFTANALYZER_H

#include <QObject>
#include <QVector>
#include <complex>

/**
 * @brief FFT analysis engine for spectrum analysis
 */
class FFTAnalyzer : public QObject
{
    Q_OBJECT

public:
    enum class WindowFunction {
        Rectangle,
        Hanning,
        Hamming,
        Blackman,
        FlatTop,
        Kaiser
    };
    Q_ENUM(WindowFunction)

    enum class ScaleType {
        Linear,
        Logarithmic,  // dB
        dBV,
        dBm
    };
    Q_ENUM(ScaleType)

    explicit FFTAnalyzer(QObject *parent = nullptr);
    ~FFTAnalyzer() override = default;

    // Configuration
    void setWindowFunction(WindowFunction window);
    WindowFunction windowFunction() const { return m_window; }
    
    void setScaleType(ScaleType scale);
    ScaleType scaleType() const { return m_scale; }
    
    void setFFTSize(int size);
    int fftSize() const { return m_fftSize; }
    
    void setOverlap(double overlap);
    double overlap() const { return m_overlap; }
    
    void setAveraging(int count);
    int averaging() const { return m_avgCount; }
    
    // Reference level for dB calculations
    void setReferenceLevel(double level);
    double referenceLevel() const { return m_refLevel; }

    // Processing
    QVector<double> compute(const QVector<double> &timeDomainData, double sampleRate);
    QVector<double> computeMagnitude(const QVector<std::complex<double>> &fftResult);
    QVector<double> computePhase(const QVector<std::complex<double>> &fftResult);
    QVector<double> computePower(const QVector<std::complex<double>> &fftResult);
    
    // Get frequency axis values
    QVector<double> frequencyAxis(double sampleRate) const;
    
    // Peak detection
    struct Peak {
        double frequency;
        double magnitude;
        int binIndex;
    };
    QVector<Peak> findPeaks(const QVector<double> &spectrum, double sampleRate,
                           int maxPeaks = 10, double threshold = -60.0);
    
    // THD calculation
    double calculateTHD(const QVector<double> &spectrum, double fundamentalFreq,
                        double sampleRate, int harmonics = 10);
    
    // SFDR calculation
    double calculateSFDR(const QVector<double> &spectrum, double fundamentalFreq,
                         double sampleRate);
    
    // SNR calculation
    double calculateSNR(const QVector<double> &spectrum, double signalFreq,
                        double sampleRate, double bandwidth = 0);

signals:
    void fftComputed(const QVector<double> &magnitude, const QVector<double> &frequencies);
    void parametersChanged();

private:
    void applyWindow(QVector<double> &data);
    QVector<double> getWindowCoefficients(int size);
    QVector<std::complex<double>> fft(const QVector<std::complex<double>> &input);
    void fftRecursive(std::complex<double> *x, int n);
    int nextPowerOfTwo(int n);
    double linearToDb(double linear);
    
    WindowFunction m_window = WindowFunction::Hanning;
    ScaleType m_scale = ScaleType::Logarithmic;
    int m_fftSize = 4096;
    double m_overlap = 0.5;
    int m_avgCount = 1;
    double m_refLevel = 1.0;  // 1V reference
    
    // Averaging buffer
    QVector<double> m_avgBuffer;
    int m_avgCounter = 0;
};

#endif // FFTANALYZER_H
