# RIGOL DHO954 Oscilloscope GUI

A full-featured graphical user interface for controlling and monitoring RIGOL DHO954 oscilloscopes using Python and PyVISA.

## Features

- **Real-time waveform display** with oscilloscope-style dark theme
- **Channel controls**: Enable/disable channels, adjust scale, offset, coupling (DC/AC/GND), and probe attenuation
- **Timebase settings**: Configure horizontal scale and offset
- **Trigger configuration**: Set trigger mode (AUTO/NORM/SING), source, level, and slope
- **Automatic measurements**: Frequency, peak-to-peak voltage, max/min voltage, RMS, average, period, pulse width
- **Screenshot capture**: Save oscilloscope screen as PNG image
- **Waveform export**: Save waveform data to CSV files
- **Auto-update**: Continuous waveform monitoring with configurable update rate
- **Configuration persistence**: Save and load user settings
- **Comprehensive logging**: Detailed logging for debugging and monitoring
- **Input validation**: Robust validation of user inputs with helpful error messages

## Project Structure

```
rigol/
├── rigol_gui.py          # Main GUI application
├── rigol_instrument.py   # Oscilloscope instrument control
├── config.py             # Configuration management
├── utils.py              # Utility functions and validation
├── requirements.txt      # Python dependencies with version constraints
├── setup.sh              # Setup script
├── config.json           # User configuration (auto-generated)
├── README.md             # This documentation
└── .gitignore           # Git ignore rules
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

## Installation

1. Clone or download this repository to your local machine.

2. Run the setup script to create a virtual environment and install dependencies:
   ```bash
   ./setup.sh
   ```

   Or set up manually:
   ```bash
   # Create virtual environment
   python3 -m venv .venv

   # Activate virtual environment
   source .venv/bin/activate

   # Install dependencies
   pip install -r requirements.txt
   ```

## Usage

1. **Connect the oscilloscope**: Ensure your RIGOL DHO954 is powered on and connected to your computer via USB or network.

2. **Activate the virtual environment** (if using one):
   ```bash
   source .venv/bin/activate
   ```

3. **Run the GUI application**:
   ```bash
   python rigol_gui.py
   ```

4. **Connect to the oscilloscope**:
   - Click the "Connect" button in the toolbar
   - The application will automatically detect and connect to available RIGOL instruments
   - Connection status is displayed in the top-right corner

5. **Configure settings**:
   - Use the left panel to adjust channel settings, timebase, and trigger parameters
   - Enable/disable channels using the checkboxes
   - Adjust vertical scale, offset, coupling, and probe ratios for each channel
   - Configure timebase scale and offset
   - Set trigger mode, source, level, and slope

6. **Control acquisition**:
   - Use RUN/STOP/SINGLE buttons to control data acquisition
   - Click "Force Trigger" to manually trigger if in NORM mode

7. **Monitor waveforms and measurements**:
   - Waveforms are displayed in real-time on the right panel
   - Measurements are shown below the waveform display
   - Click "Update Measurements" to refresh measurement values

8. **Capture data**:
   - "Screenshot" button saves the current oscilloscope screen as PNG
   - "Save Waveform" button exports waveform data to CSV format

## Configuration

The application uses a `config.json` file to store user preferences and settings. The configuration includes:

- Instrument connection settings (timeout, resource string)
- GUI preferences (window size, theme, update rates)
- Default channel, timebase, and trigger settings
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

### Dependencies
- **Import errors**: Ensure all dependencies are installed in the active Python environment
- **Tkinter not found**: Tkinter is usually included with Python; reinstall Python if missing

## Development

The application consists of several key modules:

- **`rigol_instrument.py`**: Low-level interface to the RIGOL DHO954 oscilloscope
- **`rigol_gui.py`**: Main GUI application with Tkinter interface
- **`config.py`**: Configuration management system
- **`utils.py`**: Helper functions for validation, formatting, and logging

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