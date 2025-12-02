#ifndef SCPICOMMANDS_H
#define SCPICOMMANDS_H

#include <QString>

/**
 * @brief SCPI command definitions for RIGOL oscilloscopes
 */
namespace SCPI {

// System commands
constexpr const char* IDN = "*IDN?";
constexpr const char* RST = "*RST";
constexpr const char* CLS = "*CLS";
constexpr const char* OPC = "*OPC?";

// Channel commands
inline QString channelDisplay(const QString &ch, bool on) {
    return QString(":%1:DISPlay %2").arg(ch, on ? "ON" : "OFF");
}

inline QString channelDisplayQuery(const QString &ch) {
    return QString(":%1:DISPlay?").arg(ch);
}

inline QString channelScale(const QString &ch, double scale) {
    return QString(":%1:SCALe %2").arg(ch).arg(scale);
}

inline QString channelScaleQuery(const QString &ch) {
    return QString(":%1:SCALe?").arg(ch);
}

inline QString channelOffset(const QString &ch, double offset) {
    return QString(":%1:OFFSet %2").arg(ch).arg(offset);
}

inline QString channelOffsetQuery(const QString &ch) {
    return QString(":%1:OFFSet?").arg(ch);
}

inline QString channelCoupling(const QString &ch, const QString &coupling) {
    return QString(":%1:COUPling %2").arg(ch, coupling);
}

inline QString channelCouplingQuery(const QString &ch) {
    return QString(":%1:COUPling?").arg(ch);
}

inline QString channelProbe(const QString &ch, double ratio) {
    return QString(":%1:PROBe %2").arg(ch).arg(ratio);
}

inline QString channelProbeQuery(const QString &ch) {
    return QString(":%1:PROBe?").arg(ch);
}

inline QString channelBandwidth(const QString &ch, const QString &bw) {
    return QString(":%1:BWLimit %2").arg(ch, bw);
}

inline QString channelInvert(const QString &ch, bool invert) {
    return QString(":%1:INVert %2").arg(ch, invert ? "ON" : "OFF");
}

// Timebase commands
inline QString timebaseScale(double scale) {
    return QString(":TIMebase:SCALe %1").arg(scale);
}

constexpr const char* TIMEBASE_SCALE_QUERY = ":TIMebase:SCALe?";

inline QString timebaseOffset(double offset) {
    return QString(":TIMebase:OFFSet %1").arg(offset);
}

constexpr const char* TIMEBASE_OFFSET_QUERY = ":TIMebase:OFFSet?";

inline QString timebaseMode(const QString &mode) {
    return QString(":TIMebase:MODE %1").arg(mode);
}

constexpr const char* TIMEBASE_MODE_QUERY = ":TIMebase:MODE?";

// Trigger commands
inline QString triggerEdgeSource(const QString &source) {
    return QString(":TRIGger:EDGE:SOURce %1").arg(source);
}

constexpr const char* TRIGGER_EDGE_SOURCE_QUERY = ":TRIGger:EDGE:SOURce?";

inline QString triggerEdgeLevel(double level) {
    return QString(":TRIGger:EDGE:LEVel %1").arg(level);
}

constexpr const char* TRIGGER_EDGE_LEVEL_QUERY = ":TRIGger:EDGE:LEVel?";

inline QString triggerEdgeSlope(const QString &slope) {
    return QString(":TRIGger:EDGE:SLOPe %1").arg(slope);
}

constexpr const char* TRIGGER_EDGE_SLOPE_QUERY = ":TRIGger:EDGE:SLOPe?";

inline QString triggerMode(const QString &mode) {
    return QString(":TRIGger:SWEep %1").arg(mode);
}

constexpr const char* TRIGGER_MODE_QUERY = ":TRIGger:SWEep?";

constexpr const char* TRIGGER_STATUS_QUERY = ":TRIGger:STATus?";

// Acquisition commands
constexpr const char* RUN = ":RUN";
constexpr const char* STOP = ":STOP";
constexpr const char* SINGLE = ":SINGle";
constexpr const char* FORCE = ":TFORce";

inline QString acquireType(const QString &type) {
    return QString(":ACQuire:TYPE %1").arg(type);
}

constexpr const char* ACQUIRE_TYPE_QUERY = ":ACQuire:TYPE?";

inline QString acquireAverages(int count) {
    return QString(":ACQuire:AVERages %1").arg(count);
}

constexpr const char* ACQUIRE_AVERAGES_QUERY = ":ACQuire:AVERages?";

inline QString acquireMemoryDepth(int depth) {
    return QString(":ACQuire:MDEPth %1").arg(depth);
}

constexpr const char* ACQUIRE_MEMORY_DEPTH_QUERY = ":ACQuire:MDEPth?";

// Waveform commands
inline QString waveformSource(const QString &source) {
    return QString(":WAVeform:SOURce %1").arg(source);
}

constexpr const char* WAVEFORM_SOURCE_QUERY = ":WAVeform:SOURce?";

inline QString waveformMode(const QString &mode) {
    return QString(":WAVeform:MODE %1").arg(mode);
}

constexpr const char* WAVEFORM_MODE_QUERY = ":WAVeform:MODE?";

inline QString waveformFormat(const QString &format) {
    return QString(":WAVeform:FORMat %1").arg(format);
}

constexpr const char* WAVEFORM_FORMAT_QUERY = ":WAVeform:FORMat?";

constexpr const char* WAVEFORM_PREAMBLE_QUERY = ":WAVeform:PREamble?";
constexpr const char* WAVEFORM_DATA_QUERY = ":WAVeform:DATA?";

inline QString waveformStart(int start) {
    return QString(":WAVeform:STARt %1").arg(start);
}

inline QString waveformStop(int stop) {
    return QString(":WAVeform:STOP %1").arg(stop);
}

// Measurement commands
inline QString measurementItem(const QString &item, const QString &source) {
    return QString(":MEASure:ITEM %1,%2").arg(item, source);
}

inline QString measurementItemQuery(const QString &item, const QString &source) {
    return QString(":MEASure:ITEM? %1,%2").arg(item, source);
}

constexpr const char* MEASURE_CLEAR = ":MEASure:CLEar ALL";

// Display commands
constexpr const char* DISPLAY_DATA_BMP = ":DISPlay:DATA? ON,OFF,BMP";
constexpr const char* DISPLAY_DATA_PNG = ":DISPlay:DATA? ON,OFF,PNG";

inline QString displayGrid(const QString &type) {
    return QString(":DISPlay:GRID %1").arg(type);
}

inline QString displayIntensity(int intensity) {
    return QString(":DISPlay:INTensity %1").arg(intensity);
}

constexpr const char* DISPLAY_CLEAR = ":DISPlay:CLEar";

// Math commands
inline QString mathDisplay(bool on) {
    return QString(":MATH:DISPlay %1").arg(on ? "ON" : "OFF");
}

inline QString mathOperator(const QString &op) {
    return QString(":MATH:OPERator %1").arg(op);
}

inline QString mathSource1(const QString &source) {
    return QString(":MATH:SOURce1 %1").arg(source);
}

inline QString mathSource2(const QString &source) {
    return QString(":MATH:SOURce2 %1").arg(source);
}

inline QString mathScale(double scale) {
    return QString(":MATH:SCALe %1").arg(scale);
}

// FFT commands
inline QString fftDisplay(bool on) {
    return QString(":MATH:FFT:DISPlay %1").arg(on ? "ON" : "OFF");
}

inline QString fftSource(const QString &source) {
    return QString(":MATH:FFT:SOURce %1").arg(source);
}

inline QString fftWindow(const QString &window) {
    return QString(":MATH:FFT:WINDow %1").arg(window);
}

inline QString fftScale(const QString &scale) {
    return QString(":MATH:FFT:VSCale %1").arg(scale);
}

// Storage commands
inline QString saveImage(const QString &filename) {
    return QString(":SAVe:IMAGe %1").arg(filename);
}

inline QString saveWaveform(const QString &filename) {
    return QString(":SAVe:WAVeform %1").arg(filename);
}

} // namespace SCPI

#endif // SCPICOMMANDS_H
