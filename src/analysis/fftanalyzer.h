#ifndef FFTANALYZER_H
#define FFTANALYZER_H

#include <QObject>
#include <QVector>
#include <complex>

/**
 * @brief FFT analysis engine for spectrum analysis
 *
 * The FFTAnalyzer class provides comprehensive Fast Fourier Transform analysis
 * capabilities for oscilloscope waveform data. It supports various windowing
 * functions, scaling types, and advanced spectral analysis features including
 * peak detection, total harmonic distortion (THD), spurious-free dynamic range
 * (SFDR), and signal-to-noise ratio (SNR) calculations.
 *
 * Key features:
 * - Multiple windowing functions (Rectangle, Hanning, Hamming, Blackman, etc.)
 * - Flexible scaling (Linear, Logarithmic dB, dBV, dBm)
 * - Configurable FFT size and overlap
 * - Averaging for noise reduction
 * - Real-time spectrum computation
 * - Peak detection and analysis
 * - Harmonic distortion measurements
 * - Signal quality metrics (THD, SFDR, SNR)
 *
 * The analyzer uses a recursive FFT implementation for efficiency and supports
 * both magnitude and phase spectrum computation. All processing is optimized
 * for real-time oscilloscope applications.
 */
class FFTAnalyzer : public QObject
{
    Q_OBJECT

public:
public:
    /**
     * @brief Windowing functions for FFT analysis
     */
    enum class WindowFunction {
        Rectangle, ///< Rectangular window (no windowing)
        Hanning,   ///< Hanning window for general purpose
        Hamming,   ///< Hamming window for better frequency resolution
        Blackman,  ///< Blackman window for excellent sidelobe suppression
        FlatTop,   ///< Flat-top window for accurate amplitude measurements
        Kaiser     ///< Kaiser window for adjustable sidelobe suppression
    };
    Q_ENUM(WindowFunction)

    /**
     * @brief Scaling types for spectrum display
     */
    enum class ScaleType {
        Linear,      ///< Linear amplitude scaling
        Logarithmic, ///< Logarithmic scaling in dB
        dBV,         ///< dB relative to 1V
        dBm          ///< dB relative to 1mW (50 ohm system)
    };
    Q_ENUM(ScaleType)

    /**
     * @brief Constructs an FFT analyzer with default settings
     * @param parent Parent QObject for memory management
     */
    explicit FFTAnalyzer(QObject *parent = nullptr);

    /**
     * @brief Destroys the FFT analyzer
     */
    ~FFTAnalyzer() override = default;

    // Configuration
    /**
     * @brief Sets the windowing function for FFT analysis
     * @param window The window function to apply before FFT
     */
    void setWindowFunction(WindowFunction window);

    /**
     * @brief Gets the current windowing function
     * @return Current window function setting
     */
    WindowFunction windowFunction() const { return m_window; }
    
    /**
     * @brief Sets the scaling type for spectrum display
     * @param scale The scaling type (Linear, Logarithmic, dBV, dBm)
     */
    void setScaleType(ScaleType scale);

    /**
     * @brief Gets the current scaling type
     * @return Current scale type setting
     */
    ScaleType scaleType() const { return m_scale; }
    
    /**
     * @brief Sets the FFT size (must be power of 2)
     * @param size FFT size in samples (e.g., 1024, 2048, 4096)
     */
    void setFFTSize(int size);

    /**
     * @brief Gets the current FFT size
     * @return Current FFT size in samples
     */
    int fftSize() const { return m_fftSize; }
    
    /**
     * @brief Sets the overlap factor for successive FFT frames
     * @param overlap Overlap ratio (0.0 to 0.9, where 0.5 = 50% overlap)
     */
    void setOverlap(double overlap);

    /**
     * @brief Gets the current overlap setting
     * @return Current overlap ratio
     */
    double overlap() const { return m_overlap; }
    
    /**
     * @brief Sets the number of FFTs to average for noise reduction
     * @param count Number of FFT frames to average (1 = no averaging)
     */
    void setAveraging(int count);

    /**
     * @brief Gets the current averaging count
     * @return Current averaging count
     */
    int averaging() const { return m_avgCount; }
    
    // Reference level for dB calculations
    /**
     * @brief Sets the reference level for dB calculations
     * @param level Reference level in volts (typically 1.0 for dBV)
     */
    void setReferenceLevel(double level);

    /**
     * @brief Gets the current reference level
     * @return Current reference level in volts
     */
    double referenceLevel() const { return m_refLevel; }

    // Processing
    /**
     * @brief Computes the FFT spectrum from time-domain data
     * @param timeDomainData Input time-domain samples
     * @param sampleRate Sample rate of the input data in Hz
     * @return Magnitude spectrum in the selected scale type
     */
    QVector<double> compute(const QVector<double> &timeDomainData, double sampleRate);

    /**
     * @brief Computes magnitude spectrum from complex FFT result
     * @param fftResult Complex FFT output
     * @return Magnitude values in linear scale
     */
    QVector<double> computeMagnitude(const QVector<std::complex<double>> &fftResult);

    /**
     * @brief Computes phase spectrum from complex FFT result
     * @param fftResult Complex FFT output
     * @return Phase values in radians
     */
    QVector<double> computePhase(const QVector<std::complex<double>> &fftResult);

    /**
     * @brief Computes power spectrum from complex FFT result
     * @param fftResult Complex FFT output
     * @return Power values (magnitude squared)
     */
    QVector<double> computePower(const QVector<std::complex<double>> &fftResult);
    
    // Get frequency axis values
    /**
     * @brief Generates frequency axis values for the current FFT settings
     * @param sampleRate Sample rate in Hz
     * @return Vector of frequency values in Hz
     */
    QVector<double> frequencyAxis(double sampleRate) const;
    
    // Peak detection
    /**
     * @brief Structure representing a spectral peak
     */
    struct Peak {
        double frequency; ///< Peak frequency in Hz
        double magnitude; ///< Peak magnitude in current scale
        int binIndex;     ///< FFT bin index of the peak
    };

    /**
     * @brief Finds peaks in the spectrum above a threshold
     * @param spectrum Magnitude spectrum data
     * @param sampleRate Sample rate in Hz
     * @param maxPeaks Maximum number of peaks to return
     * @param threshold Minimum magnitude threshold in dB
     * @return Vector of detected peaks
     */
    QVector<Peak> findPeaks(const QVector<double> &spectrum, double sampleRate,
                           int maxPeaks = 10, double threshold = -60.0);
    
    // THD calculation
    /**
     * @brief Calculates Total Harmonic Distortion
     * @param spectrum Magnitude spectrum data
     * @param fundamentalFreq Fundamental frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @param harmonics Number of harmonics to include (default 10)
     * @return THD value as a percentage (0-100)
     */
    double calculateTHD(const QVector<double> &spectrum, double fundamentalFreq,
                        double sampleRate, int harmonics = 10);
    
    // SFDR calculation
    /**
     * @brief Calculates Spurious-Free Dynamic Range
     * @param spectrum Magnitude spectrum data
     * @param fundamentalFreq Fundamental frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @return SFDR value in dB
     */
    double calculateSFDR(const QVector<double> &spectrum, double fundamentalFreq,
                         double sampleRate);
    
    // SNR calculation
    /**
     * @brief Calculates Signal-to-Noise Ratio
     * @param spectrum Magnitude spectrum data
     * @param signalFreq Signal frequency in Hz
     * @param sampleRate Sample rate in Hz
     * @param bandwidth Bandwidth for noise calculation in Hz (0 = full spectrum)
     * @return SNR value in dB
     */
    double calculateSNR(const QVector<double> &spectrum, double signalFreq,
                        double sampleRate, double bandwidth = 0);

signals:
    /**
     * @brief Emitted when FFT computation is completed
     * @param magnitude Magnitude spectrum data
     * @param frequencies Corresponding frequency values
     */
    void fftComputed(const QVector<double> &magnitude, const QVector<double> &frequencies);

    /**
     * @brief Emitted when analysis parameters change
     */
    void parametersChanged();

private:
    /**
     * @brief Applies the selected window function to the data
     * @param data Time-domain data to window (modified in-place)
     */
    void applyWindow(QVector<double> &data);

    /**
     * @brief Generates window coefficients for the current window function
     * @param size Number of coefficients to generate
     * @return Vector of window coefficients
     */
    QVector<double> getWindowCoefficients(int size);

    /**
     * @brief Performs FFT on complex input data
     * @param input Complex input data
     * @return Complex FFT result
     */
    QVector<std::complex<double>> fft(const QVector<std::complex<double>> &input);

    /**
     * @brief Recursive FFT implementation (Cooley-Tukey algorithm)
     * @param x Complex data array (modified in-place)
     * @param n Size of the FFT (must be power of 2)
     */
    void fftRecursive(std::complex<double> *x, int n);

    /**
     * @brief Finds the next power of two greater than or equal to n
     * @param n Input value
     * @return Smallest power of 2 >= n
     */
    int nextPowerOfTwo(int n);

    /**
     * @brief Converts linear magnitude to decibels
     * @param linear Linear magnitude value
     * @return Value in dB
     */
    double linearToDb(double linear);
    
    /// @brief Current windowing function
    WindowFunction m_window = WindowFunction::Hanning;

    /// @brief Current scaling type
    ScaleType m_scale = ScaleType::Logarithmic;

    /// @brief Current FFT size (power of 2)
    int m_fftSize = 4096;

    /// @brief Overlap ratio for successive frames (0.0-0.9)
    double m_overlap = 0.5;

    /// @brief Number of FFTs to average
    int m_avgCount = 1;

    /// @brief Reference level for dB calculations in volts
    double m_refLevel = 1.0;  // 1V reference
    
    // Averaging buffer
    /// @brief Buffer for accumulating averaged FFT results
    QVector<double> m_avgBuffer;

    /// @brief Counter for averaging frames
    int m_avgCounter = 0;
};

#endif // FFTANALYZER_H
