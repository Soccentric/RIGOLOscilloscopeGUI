# RIGOL Oscilloscope GUI

A professional-grade Qt6-based GUI application for controlling RIGOL MSO/DS series oscilloscopes over TCP/IP using SCPI commands.

![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)
![Qt](https://img.shields.io/badge/Qt-6.x-green.svg)
![C++](https://img.shields.io/badge/C++-17-orange.svg)
![License](https://img.shields.io/badge/license-MIT-lightgrey.svg)

## Features

### 🔌 Connectivity
- TCP/IP connection to RIGOL oscilloscopes
- Automatic device discovery on local network
- Connection presets and recent connections
- Auto-reconnection on connection loss
- Connection status monitoring

### 📊 Waveform Display
- **OpenGL-accelerated rendering** for smooth, high-performance display
- Four-channel support with independent color coding
- Adjustable grid with major/minor divisions
- Mouse-based zoom and pan
- Persistence mode for eye diagrams
- Trigger level indicator
- Time and voltage cursors

### 📏 Measurements
- **20+ automatic measurements** including:
  - Voltage: Vpp, Vmax, Vmin, Vavg, Vrms, Vtop, Vbase
  - Time: Frequency, Period, Rise Time, Fall Time, Duty Cycle
  - Advanced: Phase, Delay, Overshoot, Undershoot
- Real-time statistics (min/max/average/standard deviation)
- Measurement history and trending

### 🎚️ Channel Controls
- Voltage scale: 1mV/div to 10V/div
- Vertical offset adjustment
- Coupling: DC, AC, GND
- Probe attenuation: 1X, 10X, 100X, 1000X
- Bandwidth limiting
- Invert function

### ⚡ Triggering
- **Trigger modes**: Auto, Normal, Single
- **Trigger types**: Edge, Pulse, Video, I2C, SPI
- Adjustable trigger level with slider
- 50% trigger button
- Trigger coupling: DC, AC, LF Reject, HF Reject

### 📈 FFT Spectrum Analysis
- Real-time FFT computation
- **Window functions**: Rectangle, Hanning, Hamming, Blackman, Flat Top, Kaiser
- Linear/logarithmic scale
- Peak detection
- THD, SFDR, SNR calculations
- Averaging modes

### 🔍 Protocol Decoding
- **I2C**: Address, Read/Write, Data, ACK/NAK detection
- **SPI**: CPOL/CPHA modes, MSB/LSB first, variable word size
- **UART**: Configurable baud rate, data bits, parity, stop bits
- Frame-by-frame decode visualization
- Error detection

### 💾 Data Export
- **CSV/TSV**: Spreadsheet-compatible format
- **Binary**: Raw sample data for processing
- **MATLAB**: .mat file format (Level 4)
- **WAV**: Audio format for playback
- **VCD**: Value Change Dump for digital signals
- **JSON**: Structured data with metadata
- **PNG**: Screenshot capture

### 🎨 User Interface
- Modern dark theme with VS Code-inspired styling
- Dockable panels for flexible layout
- Status bar with connection and acquisition info
- Keyboard shortcuts for common operations
- High-DPI display support

## Requirements

### Build Requirements
- CMake 3.16+
- Qt 6.x with modules:
  - Core, Gui, Widgets
  - Network
  - Charts
  - OpenGLWidgets, OpenGL
- C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)

### Runtime Requirements
- OpenGL 3.3+ compatible graphics
- Network access to oscilloscope

## Building

### Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt install build-essential cmake qt6-base-dev qt6-charts-dev libgl1-mesa-dev

# Clone and build
git clone https://github.com/your-repo/RIGOLOscilloscopeGUI.git
cd RIGOLOscilloscopeGUI
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### macOS

```bash
# Install dependencies
brew install cmake qt@6

# Build
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
make -j$(sysctl -n hw.ncpu)
```

### Windows

```powershell
# Using Visual Studio
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
cmake --build . --config Release
```

## Usage

1. **Connect to Oscilloscope**
   - Go to `File → Connect` or press `Ctrl+Shift+C`
   - Enter IP address or scan network
   - Click Connect

2. **View Waveforms**
   - Enable channels using channel buttons (CH1-CH4)
   - Adjust timebase using Time/Div control
   - Adjust voltage scale per channel

3. **Take Measurements**
   - Open Measurements panel from `View` menu
   - Click "Add Measurement" to select measurement types
   - View statistics in the Measurements dock

4. **Analyze Spectrum**
   - Open FFT panel from `View → FFT`
   - Select source channel and window function
   - View frequency spectrum and harmonics

5. **Decode Protocols**
   - Open Protocol Decoder from `Analyze` menu
   - Select protocol type and configure parameters
   - Assign channels (SDA/SCL for I2C, etc.)

6. **Export Data**
   - Go to `File → Export`
   - Select format and options
   - Choose save location

## Architecture

```
src/
├── main.cpp                 # Application entry point
├── communication/           # Device communication layer
│   ├── deviceconnection     # TCP/IP connection management
│   ├── scpicommands         # SCPI command definitions
│   └── waveformprovider     # Waveform acquisition
├── core/                    # Core functionality
│   ├── scopechannel         # Channel properties
│   ├── scopesettings        # Settings persistence
│   ├── measurementengine    # Measurement calculations
│   └── cursormanager        # Cursor management
├── ui/                      # User interface
│   ├── mainwindow           # Main application window
│   ├── waveformdisplay      # OpenGL waveform widget
│   ├── channelcontrolpanel  # Channel settings panel
│   ├── triggerpanel         # Trigger configuration
│   ├── measurementpanel     # Measurement display
│   └── connectiondialog     # Connection dialog
├── analysis/                # Analysis tools
│   ├── fftanalyzer          # FFT spectrum analysis
│   └── protocoldecoder      # Protocol decoding
└── utils/                   # Utilities
    └── dataexporter         # Data export functions

scripts/                     # Build and maintenance scripts
├── format.sh                # Code formatting script
└── lint.sh                  # Code linting script

docs/                        # Documentation
└── MSO5000_ProgrammingGuide_EN-V2.0.pdf  # RIGOL programming guide
```

## Development

### Code Formatting

This project uses clang-format for consistent code formatting. A `.clang-format` file is provided in the root directory.

To format all source files:
```bash
./scripts/format.sh
```

Or manually:
```bash
find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

### Code Linting

cpplint is used for static code analysis. Configuration is in `CPPLINT.cfg`.

To lint all source files:
```bash
./scripts/lint.sh
```

Or manually:
```bash
find src -name "*.cpp" -o -name "*.h" | xargs cpplint
```

## Supported Oscilloscopes

Tested with:
- RIGOL DS1054Z
- RIGOL DS1104Z
- RIGOL MSO5074
- RIGOL MSO5104

Should work with any RIGOL oscilloscope supporting SCPI over TCP/IP (port 5555).

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+Shift+C` | Connect to oscilloscope |
| `Ctrl+R` | Run/Stop acquisition |
| `Ctrl+S` | Single trigger |
| `Ctrl+A` | Auto-scale |
| `Ctrl+E` | Export data |
| `1-4` | Toggle channels |
| `+/-` | Zoom in/out |
| `Arrow keys` | Pan waveform |
| `Ctrl+Z` | Undo |
| `Ctrl+Shift+Z` | Redo |
| `F1` | Help |
| `F11` | Fullscreen |

## Contributing

Contributions are welcome! Please read our contributing guidelines before submitting pull requests.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- RIGOL Technologies for their oscilloscope SCPI documentation
- Qt Project for the excellent framework
- OpenGL community for graphics rendering resources