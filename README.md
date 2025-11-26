# RIGOL DHO954 Oscilloscope GUI

A full-featured graphical user interface for controlling and monitoring RIGOL DHO954 oscilloscopes using Python and PyVISA.

## Features

- **Real-time waveform display** with oscilloscope-style dark theme
- **Channel controls**: Enable/disable channels, adjust scale, offset, coupling (DC/AC/GND), and probe attenuation
- **Logic Analyzer (Digital Channels)**:
  - 16 digital channels (D0-D15) with individual enable/disable
  - Multiple threshold types (TTL, CMOS5, CMOS3, ECL, LVTTL, LVCMOS3, LVCMOS2, CUSTOM)
  - Custom threshold voltage setting
  - Channel labeling (up to 4 characters per channel)
  - Bulk channel operations (enable all, disable all, enable D0-D7, enable D8-D15)
  - Digital waveform display with step visualization
  - Integrated with analog waveform display
- **Timebase settings**: Configure horizontal scale and offset
- **Trigger configuration**: Set trigger mode (AUTO/NORM/SING), source, level, and slope
- **Automatic measurements**: Frequency, peak-to-peak voltage, max/min voltage, RMS, average, period, pulse width
- **Screenshot capture**: Save oscilloscope screen as PNG image
- **Waveform export**: Save waveform data (analog and digital) to CSV files
- **Auto-update**: Continuous waveform monitoring with configurable update rate
- **Configuration persistence**: Save and load user settings (including logic analyzer preferences)
- **Comprehensive logging**: Detailed logging for debugging and monitoring
- **Input validation**: Robust validation of user inputs with helpful error messages

## Project Structure

```
RIGOLOscilloscopeGUI/
├── src/
│   ├── rigol_gui.py          # Main GUI application
│   ├── rigol_instrument.py   # Oscilloscope instrument control
│   ├── config.py             # Configuration management
│   ├── utils.py              # Utility functions and validation
│   └── test_components.py    # Test components
├── requirements.txt          # Python dependencies with version constraints
├── setup.sh                  # Setup script
├── Makefile                  # Build automation
├── config.json               # User configuration (auto-generated)
├── README.md                 # This documentation
└── .gitignore               # Git ignore rules
```

## Requirements

- Python 3.6 or higher
- RIGOL DHO954 oscilloscope
- USB connection or network interface to the oscilloscope

## Dependencies

- `pyvisa>=1.13.0` - VISA library for instrument communication
- `pyvisa-py>=0.7.0` - Pure Python VISA implementation
- `matplotlib>=3.8.0` - Plotting library for waveform display
- `numpy>=1.26.0` - Numerical computing library
- `tkinter` - GUI framework (usually included with Python installations)
- `pyinstaller>=6.0.0` - For building standalone executables (optional)

## Installation

1. Clone or download this repository to your local machine.

2. Use the Makefile to set up the environment:

   ```bash
   make all
   ```

   This creates a virtual environment and installs all dependencies.

   Or set up manually:

   ```bash
   # Create virtual environment
   python3 -m venv .venv

   # Activate virtual environment
   source .venv/bin/activate

   # Install dependencies
   pip install -r requirements.txt
   ```

## Building Standalone Executable

To create a standalone executable that doesn't require Python installation:

1. Ensure dependencies are installed (see Installation above).

2. Build the executable:

   ```bash
   make build
   ```

   This creates a single executable file `dist/rigol_gui` (Linux/Mac) or `dist/rigol_gui.exe` (Windows) that can be run directly.

## Makefile Targets

The project includes a comprehensive Makefile for common tasks:

- `make all` - Create virtual environment and install dependencies (default)
- `make venv` - Create virtual environment
- `make install` - Install Python dependencies
- `make run` - Run the GUI application
- `make test` - Run tests
- `make clean` - Clean cache and build artifacts
- `make distclean` - Deep clean including virtual environment
- `make uninstall` - Uninstall Python packages
- `make build` - Build standalone executable
- `make help` - Show available targets

## Usage

1. **Connect the oscilloscope**: Ensure your RIGOL DHO954 is powered on and connected to your computer via USB or network.

2. **Run the GUI application**:

   ```bash
   make run
   ```

   Or manually:

   ```bash
   # Activate the virtual environment
   source .venv/bin/activate

   # Run the application
   python src/rigol_gui.py
   ```

3. **Connect to the oscilloscope**:

   - Click the "Connect" button in the toolbar
   - The application will automatically detect and connect to available RIGOL instruments
   - Connection status is displayed in the top-right corner

4. **Configure settings**:

   - Use the left panel to adjust channel settings, timebase, and trigger parameters
   - Enable/disable channels using the checkboxes
   - Adjust vertical scale, offset, coupling, and probe ratios for each channel
   - **Logic Analyzer**:
     - Check "Enable Logic Analyzer" to activate digital channels
     - Select threshold type from dropdown (TTL, CMOS, ECL, etc.)
     - For custom threshold, select "CUSTOM" and enter voltage level
     - Enable individual digital channels (D0-D15) by checking their boxes
     - Customize channel labels (max 4 characters) for better identification
     - Use bulk buttons to quickly enable/disable groups of channels
   - Configure timebase scale and offset
   - Set trigger mode, source, level, and slope

5. **Control acquisition**:

   - Use RUN/STOP/SINGLE buttons to control data acquisition
   - Click "Force Trigger" to manually trigger if in NORM mode

6. **Monitor waveforms and measurements**:

   - Analog waveforms are displayed in the top panel
   - Digital waveforms are displayed in the bottom panel (when LA is enabled)
   - Digital signals show as step waveforms with logic levels
   - Measurements are shown below the waveform display
   - Click "Update Measurements" to refresh measurement values

7. **Capture data**:
   - "Screenshot" button saves the current oscilloscope screen as PNG
   - "Save Waveform" button exports both analog and digital waveform data to CSV format

## Configuration

The application uses a `config.json` file to store user preferences and settings. The configuration includes:

- Instrument connection settings (timeout, resource string)
- GUI preferences (window size, theme, update rates)
- Default channel, timebase, and trigger settings
- Logic analyzer settings (threshold type, custom levels, channel labels)
- Logging configuration

Settings are automatically saved when the application closes and loaded on startup.

## Logging

The application provides comprehensive logging for debugging and monitoring:

- Log levels can be configured (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- Logs can be written to a file (`rigol_gui.log` by default)
- Console output for immediate feedback

## Troubleshooting

### Connection Issues

- **No instrument found**: Ensure the oscilloscope is powered on and properly connected
- **USB connection**: Install appropriate USB drivers for your operating system
- **Network connection**: Verify IP address and network settings on the oscilloscope
- **Permission issues**: On Linux, you may need to run with sudo or configure udev rules for USB access

### GUI Issues

- **Blank waveform display**: Check that at least one channel is enabled
- **Slow performance**: Reduce the number of data points or update rate
- **Measurement errors**: Ensure valid signals are present on enabled channels
- **Digital channels not showing**: Verify that "Enable Logic Analyzer" is checked and individual digital channels are enabled
- **Digital threshold issues**: Make sure the threshold type matches your signal levels

### Dependencies

- **Import errors**: Ensure all dependencies are installed in the active Python environment
- **Tkinter not found**: Tkinter is usually included with Python; reinstall Python if missing

## Development

The application consists of several key modules:

- **`rigol_instrument.py`**: Low-level interface to the RIGOL DHO954 oscilloscope (analog and digital channels)
- **`rigol_gui.py`**: Main GUI application with Tkinter interface (includes logic analyzer controls)
- **`config.py`**: Configuration management system
- **`utils.py`**: Helper functions for validation, formatting, and logging (includes digital channel validation)

### Code Quality Features

- Type hints throughout the codebase for better IDE support
- Comprehensive error handling with user-friendly messages
- Input validation to prevent invalid commands
- Proper logging for debugging and monitoring
- Modular architecture for maintainability

### Adding New Features

1. Extend the `RigolDHO954` class in `rigol_instrument.py` for new oscilloscope commands
2. Add UI elements in the `OscilloscopeGUI.setup_*` methods in `rigol_gui.py`
3. Implement callback functions for new controls
4. Update configuration defaults in `config.py` if needed
5. Add validation functions in `utils.py` for new input types

**Example - Logic Analyzer Implementation:**

- Added `set_digital_display()`, `get_digital_data()`, and other digital channel methods to `rigol_instrument.py`
- Created `setup_logic_analyzer_controls()` in `rigol_gui.py` for UI elements
- Added digital channel variables and line objects for plotting
- Included logic analyzer defaults in `config.py`
- Added validation functions `validate_digital_channel()`, `validate_digital_threshold()`, etc. in `utils.py`

### Testing

While full integration testing requires hardware, you can test individual components:

```python
# Test configuration
from config import Config
config = Config()
config.set('gui.window_size', '1200x800')
config.save()

# Test utilities
from utils import format_measurement_value, validate_scale_value
print(format_measurement_value(1000.0, 'FREQ'))  # "1.000 kHz"
print(validate_scale_value(0.001))  # True
```

## License

This project is provided as-is for educational and research purposes. Please check your local regulations regarding instrument control software.

## Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

## Support

For issues specific to the RIGOL DHO954 oscilloscope, refer to the official RIGOL documentation and user manual.
