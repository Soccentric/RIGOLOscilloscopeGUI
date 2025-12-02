#include "fftanalyzer.h"
#include <cmath>
#include <algorithm>
#include <numeric>

FFTAnalyzer::FFTAnalyzer(QObject *parent)
    : QObject(parent)
{
}

void FFTAnalyzer::setWindowFunction(WindowFunction window)
{
    if (m_window != window) {
        m_window = window;
        emit parametersChanged();
    }
}

void FFTAnalyzer::setScaleType(ScaleType scale)
{
    if (m_scale != scale) {
        m_scale = scale;
        emit parametersChanged();
    }
}

void FFTAnalyzer::setFFTSize(int size)
{
    int newSize = nextPowerOfTwo(size);
    if (m_fftSize != newSize) {
        m_fftSize = newSize;
        m_avgBuffer.clear();
        m_avgCounter = 0;
        emit parametersChanged();
    }
}

void FFTAnalyzer::setOverlap(double overlap)
{
    m_overlap = qBound(0.0, overlap, 0.99);
}

void FFTAnalyzer::setAveraging(int count)
{
    m_avgCount = qMax(1, count);
    m_avgBuffer.clear();
    m_avgCounter = 0;
}

void FFTAnalyzer::setReferenceLevel(double level)
{
    m_refLevel = level;
}

QVector<double> FFTAnalyzer::compute(const QVector<double> &timeDomainData, double sampleRate)
{
    if (timeDomainData.size() < m_fftSize) {
        return QVector<double>();
    }
    
    // Prepare data
    QVector<double> data(m_fftSize);
    for (int i = 0; i < m_fftSize; ++i) {
        data[i] = timeDomainData[i];
    }
    
    // Apply window
    applyWindow(data);
    
    // Convert to complex
    QVector<std::complex<double>> complexData(m_fftSize);
    for (int i = 0; i < m_fftSize; ++i) {
        complexData[i] = std::complex<double>(data[i], 0.0);
    }
    
    // Compute FFT
    QVector<std::complex<double>> fftResult = fft(complexData);
    
    // Compute magnitude
    QVector<double> magnitude = computeMagnitude(fftResult);
    
    // Apply averaging
    if (m_avgCount > 1) {
        if (m_avgBuffer.isEmpty()) {
            m_avgBuffer = magnitude;
            m_avgCounter = 1;
        } else {
            double alpha = 1.0 / m_avgCount;
            for (int i = 0; i < magnitude.size(); ++i) {
                m_avgBuffer[i] = (1.0 - alpha) * m_avgBuffer[i] + alpha * magnitude[i];
            }
            m_avgCounter = qMin(m_avgCounter + 1, m_avgCount);
        }
        magnitude = m_avgBuffer;
    }
    
    // Convert to dB if needed
    if (m_scale != ScaleType::Linear) {
        for (double &val : magnitude) {
            val = linearToDb(val / m_refLevel);
        }
    }
    
    // Only return positive frequencies
    int halfSize = m_fftSize / 2;
    QVector<double> result(halfSize);
    for (int i = 0; i < halfSize; ++i) {
        result[i] = magnitude[i];
    }
    
    QVector<double> frequencies = frequencyAxis(sampleRate);
    emit fftComputed(result, frequencies);
    
    return result;
}

QVector<double> FFTAnalyzer::computeMagnitude(const QVector<std::complex<double>> &fftResult)
{
    QVector<double> magnitude(fftResult.size());
    double scale = 2.0 / fftResult.size();
    
    for (int i = 0; i < fftResult.size(); ++i) {
        magnitude[i] = std::abs(fftResult[i]) * scale;
    }
    
    // DC and Nyquist components are not doubled
    magnitude[0] /= 2.0;
    if (fftResult.size() % 2 == 0) {
        magnitude[fftResult.size() / 2] /= 2.0;
    }
    
    return magnitude;
}

QVector<double> FFTAnalyzer::computePhase(const QVector<std::complex<double>> &fftResult)
{
    QVector<double> phase(fftResult.size());
    
    for (int i = 0; i < fftResult.size(); ++i) {
        phase[i] = std::arg(fftResult[i]) * 180.0 / M_PI;
    }
    
    return phase;
}

QVector<double> FFTAnalyzer::computePower(const QVector<std::complex<double>> &fftResult)
{
    QVector<double> power(fftResult.size());
    double scale = 1.0 / (fftResult.size() * fftResult.size());
    
    for (int i = 0; i < fftResult.size(); ++i) {
        power[i] = std::norm(fftResult[i]) * scale;
    }
    
    return power;
}

QVector<double> FFTAnalyzer::frequencyAxis(double sampleRate) const
{
    int halfSize = m_fftSize / 2;
    QVector<double> frequencies(halfSize);
    
    double binWidth = sampleRate / m_fftSize;
    for (int i = 0; i < halfSize; ++i) {
        frequencies[i] = i * binWidth;
    }
    
    return frequencies;
}

QVector<FFTAnalyzer::Peak> FFTAnalyzer::findPeaks(const QVector<double> &spectrum, 
                                                    double sampleRate,
                                                    int maxPeaks, 
                                                    double threshold)
{
    QVector<Peak> peaks;
    double binWidth = sampleRate / (spectrum.size() * 2);
    
    // Find local maxima above threshold
    for (int i = 2; i < spectrum.size() - 2; ++i) {
        if (spectrum[i] > threshold &&
            spectrum[i] > spectrum[i-1] && spectrum[i] > spectrum[i-2] &&
            spectrum[i] > spectrum[i+1] && spectrum[i] > spectrum[i+2]) {
            
            Peak peak;
            peak.binIndex = i;
            peak.magnitude = spectrum[i];
            
            // Quadratic interpolation for better frequency estimate
            double y0 = spectrum[i-1];
            double y1 = spectrum[i];
            double y2 = spectrum[i+1];
            double delta = 0.5 * (y0 - y2) / (y0 - 2*y1 + y2);
            
            peak.frequency = (i + delta) * binWidth;
            peak.magnitude = y1 - 0.25 * (y0 - y2) * delta;
            
            peaks.append(peak);
        }
    }
    
    // Sort by magnitude (descending)
    std::sort(peaks.begin(), peaks.end(), [](const Peak &a, const Peak &b) {
        return a.magnitude > b.magnitude;
    });
    
    // Limit to maxPeaks
    if (peaks.size() > maxPeaks) {
        peaks.resize(maxPeaks);
    }
    
    return peaks;
}

double FFTAnalyzer::calculateTHD(const QVector<double> &spectrum, 
                                  double fundamentalFreq,
                                  double sampleRate, 
                                  int harmonics)
{
    double binWidth = sampleRate / (spectrum.size() * 2);
    
    // Find fundamental
    int fundBin = static_cast<int>(fundamentalFreq / binWidth + 0.5);
    if (fundBin <= 0 || fundBin >= spectrum.size()) {
        return 0.0;
    }
    
    double fundPower = std::pow(10.0, spectrum[fundBin] / 10.0);
    double harmonicPower = 0.0;
    
    // Sum harmonic powers
    for (int h = 2; h <= harmonics; ++h) {
        int harmBin = fundBin * h;
        if (harmBin >= spectrum.size()) break;
        
        double power = std::pow(10.0, spectrum[harmBin] / 10.0);
        harmonicPower += power;
    }
    
    // THD = sqrt(sum of harmonic powers) / fundamental amplitude
    return 100.0 * std::sqrt(harmonicPower / fundPower);
}

double FFTAnalyzer::calculateSFDR(const QVector<double> &spectrum,
                                   double fundamentalFreq,
                                   double sampleRate)
{
    double binWidth = sampleRate / (spectrum.size() * 2);
    int fundBin = static_cast<int>(fundamentalFreq / binWidth + 0.5);
    
    if (fundBin <= 0 || fundBin >= spectrum.size()) {
        return 0.0;
    }
    
    double fundLevel = spectrum[fundBin];
    
    // Find highest spur (excluding DC and fundamental)
    double maxSpur = -200.0;
    int spurWidth = 5;  // Bins around fundamental to exclude
    
    for (int i = 5; i < spectrum.size(); ++i) {
        if (std::abs(i - fundBin) > spurWidth) {
            maxSpur = std::max(maxSpur, spectrum[i]);
        }
    }
    
    return fundLevel - maxSpur;
}

double FFTAnalyzer::calculateSNR(const QVector<double> &spectrum,
                                  double signalFreq,
                                  double sampleRate,
                                  double bandwidth)
{
    double binWidth = sampleRate / (spectrum.size() * 2);
    int sigBin = static_cast<int>(signalFreq / binWidth + 0.5);
    
    if (sigBin <= 0 || sigBin >= spectrum.size()) {
        return 0.0;
    }
    
    // Signal power (few bins around signal)
    int sigWidth = 3;
    double signalPower = 0.0;
    for (int i = sigBin - sigWidth; i <= sigBin + sigWidth; ++i) {
        if (i >= 0 && i < spectrum.size()) {
            signalPower += std::pow(10.0, spectrum[i] / 10.0);
        }
    }
    
    // Noise power (rest of spectrum)
    double noisePower = 0.0;
    int noiseCount = 0;
    
    int startBin = (bandwidth > 0) ? 0 : 5;
    int endBin = (bandwidth > 0) ? static_cast<int>(bandwidth / binWidth) : static_cast<int>(spectrum.size());
    endBin = std::min(endBin, static_cast<int>(spectrum.size()));
    
    for (int i = startBin; i < endBin; ++i) {
        if (std::abs(i - sigBin) > sigWidth) {
            noisePower += std::pow(10.0, spectrum[i] / 10.0);
            noiseCount++;
        }
    }
    
    if (noiseCount == 0 || noisePower == 0) return 0.0;
    
    return 10.0 * std::log10(signalPower / noisePower);
}

void FFTAnalyzer::applyWindow(QVector<double> &data)
{
    QVector<double> window = getWindowCoefficients(data.size());
    
    for (int i = 0; i < data.size(); ++i) {
        data[i] *= window[i];
    }
}

QVector<double> FFTAnalyzer::getWindowCoefficients(int size)
{
    QVector<double> window(size);
    
    switch (m_window) {
        case WindowFunction::Rectangle:
            for (int i = 0; i < size; ++i) {
                window[i] = 1.0;
            }
            break;
            
        case WindowFunction::Hanning:
            for (int i = 0; i < size; ++i) {
                window[i] = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (size - 1)));
            }
            break;
            
        case WindowFunction::Hamming:
            for (int i = 0; i < size; ++i) {
                window[i] = 0.54 - 0.46 * std::cos(2.0 * M_PI * i / (size - 1));
            }
            break;
            
        case WindowFunction::Blackman:
            for (int i = 0; i < size; ++i) {
                double n = 2.0 * M_PI * i / (size - 1);
                window[i] = 0.42 - 0.5 * std::cos(n) + 0.08 * std::cos(2.0 * n);
            }
            break;
            
        case WindowFunction::FlatTop:
            for (int i = 0; i < size; ++i) {
                double n = 2.0 * M_PI * i / (size - 1);
                window[i] = 0.21557895 - 0.41663158 * std::cos(n) +
                           0.277263158 * std::cos(2.0 * n) -
                           0.083578947 * std::cos(3.0 * n) +
                           0.006947368 * std::cos(4.0 * n);
            }
            break;
            
        case WindowFunction::Kaiser:
            // Kaiser window with beta = 5
            {
                double beta = 5.0;
                double alpha = (size - 1) / 2.0;
                auto bessel_i0 = [](double x) -> double {
                    double sum = 1.0;
                    double term = 1.0;
                    for (int k = 1; k <= 25; ++k) {
                        term *= (x / (2.0 * k)) * (x / (2.0 * k));
                        sum += term;
                    }
                    return sum;
                };
                
                double denom = bessel_i0(beta);
                for (int i = 0; i < size; ++i) {
                    double r = (i - alpha) / alpha;
                    double arg = beta * std::sqrt(1.0 - r * r);
                    window[i] = bessel_i0(arg) / denom;
                }
            }
            break;
    }
    
    return window;
}

QVector<std::complex<double>> FFTAnalyzer::fft(const QVector<std::complex<double>> &input)
{
    int n = input.size();
    
    // Make sure size is power of 2
    int newSize = nextPowerOfTwo(n);
    
    QVector<std::complex<double>> data(newSize, std::complex<double>(0, 0));
    for (int i = 0; i < n; ++i) {
        data[i] = input[i];
    }
    
    // Bit-reversal permutation
    int j = 0;
    for (int i = 0; i < newSize - 1; ++i) {
        if (i < j) {
            std::swap(data[i], data[j]);
        }
        int k = newSize / 2;
        while (k <= j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }
    
    // Cooley-Tukey FFT
    for (int len = 2; len <= newSize; len *= 2) {
        double angle = -2.0 * M_PI / len;
        std::complex<double> wlen(std::cos(angle), std::sin(angle));
        
        for (int i = 0; i < newSize; i += len) {
            std::complex<double> w(1, 0);
            for (int j = 0; j < len / 2; ++j) {
                std::complex<double> u = data[i + j];
                std::complex<double> v = data[i + j + len/2] * w;
                data[i + j] = u + v;
                data[i + j + len/2] = u - v;
                w *= wlen;
            }
        }
    }
    
    return data;
}

int FFTAnalyzer::nextPowerOfTwo(int n)
{
    int power = 1;
    while (power < n) {
        power *= 2;
    }
    return power;
}

double FFTAnalyzer::linearToDb(double linear)
{
    if (linear <= 0) return -200.0;
    return 20.0 * std::log10(linear);
}
