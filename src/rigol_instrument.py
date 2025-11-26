"""
RIGOL DHO954 Oscilloscope Instrument Control Module
Provides low-level interface to RIGOL DHO954 oscilloscope via PyVISA

Author: Sandesh Ghimire <sandesh@soccentric.com>
"""

import pyvisa
import time
import numpy as np
import logging

logger = logging.getLogger(__name__)


class RigolDHO954:
    """RIGOL DHO954 Oscilloscope Control Class"""

    def __init__(self, resource_string: str = None, timeout: int = 10000):
        """
        Initialize connection to RIGOL DHO954 oscilloscope

        Args:
            resource_string: VISA resource string. If None, auto-detect RIGOL instrument
            timeout: Connection timeout in milliseconds
        """
        self.rm = pyvisa.ResourceManager()
        self.timeout = timeout

        if resource_string is None:
            resources = self.rm.list_resources()
            for res in resources:
                if 'RIGOL' in res.upper() or '0x1AB1' in res:
                    resource_string = res
                    break

            if resource_string is None:
                raise ConnectionError("No RIGOL instrument found. Please check connections and drivers.")

        logger.info(f"Connecting to oscilloscope at {resource_string}")
        self.inst = self.rm.open_resource(resource_string)
        self.inst.timeout = self.timeout
        self.idn = self.query("*IDN?")
        logger.info(f"Connected to: {self.idn}")

    def write(self, command: str) -> None:
        """Send command to oscilloscope"""
        logger.debug(f"Sending command: {command}")
        self.inst.write(command)

    def query(self, command: str) -> str:
        """Send query and return response"""
        logger.debug(f"Sending query: {command}")
        return self.inst.query(command).strip()

    def reset(self) -> None:
        """Reset oscilloscope to default state"""
        logger.info("Resetting oscilloscope")
        self.write("*RST")
        time.sleep(2)

    def autoscale(self) -> None:
        """Perform autoscale"""
        logger.info("Performing autoscale")
        self.write(":AUT")
        time.sleep(2)

    def set_channel_display(self, channel: int, state: bool) -> None:
        """Turn channel on/off"""
        self.write(f":CHAN{channel}:DISP {1 if state else 0}")
        logger.debug(f"Channel {channel} display set to {state}")

    def set_channel_scale(self, channel: int, scale: float) -> None:
        """Set vertical scale in V/div"""
        self.write(f":CHAN{channel}:SCAL {scale}")
        logger.debug(f"Channel {channel} scale set to {scale} V/div")

    def set_channel_offset(self, channel: int, offset: float) -> None:
        """Set vertical offset in volts"""
        self.write(f":CHAN{channel}:OFFS {offset}")
        logger.debug(f"Channel {channel} offset set to {offset} V")

    def set_channel_coupling(self, channel: int, coupling: str) -> None:
        """Set channel coupling (DC, AC, GND)"""
        valid_couplings = ['DC', 'AC', 'GND']
        if coupling not in valid_couplings:
            raise ValueError(f"Invalid coupling. Must be one of {valid_couplings}")
        self.write(f":CHAN{channel}:COUP {coupling}")
        logger.debug(f"Channel {channel} coupling set to {coupling}")

    def set_channel_probe(self, channel: int, ratio: float) -> None:
        """Set probe attenuation ratio"""
        self.write(f":CHAN{channel}:PROB {ratio}")
        logger.debug(f"Channel {channel} probe ratio set to {ratio}")

    def set_timebase_scale(self, scale: float) -> None:
        """Set horizontal timebase in s/div"""
        self.write(f":TIM:SCAL {scale}")
        logger.debug(f"Timebase scale set to {scale} s/div")

    def set_timebase_offset(self, offset: float) -> None:
        """Set horizontal offset in seconds"""
        self.write(f":TIM:OFFS {offset}")
        logger.debug(f"Timebase offset set to {offset} s")

    def set_trigger_mode(self, mode: str) -> None:
        """Set trigger mode (AUTO, NORM, SING)"""
        valid_modes = ['AUTO', 'NORM', 'SING']
        if mode not in valid_modes:
            raise ValueError(f"Invalid trigger mode. Must be one of {valid_modes}")
        self.write(f":TRIG:MODE {mode}")
        logger.debug(f"Trigger mode set to {mode}")

    def set_trigger_source(self, source: str) -> None:
        """Set trigger source"""
        self.write(f":TRIG:EDGE:SOUR {source}")
        logger.debug(f"Trigger source set to {source}")

    def set_trigger_level(self, level: float) -> None:
        """Set trigger level in volts"""
        self.write(f":TRIG:EDGE:LEV {level}")
        logger.debug(f"Trigger level set to {level} V")

    def set_trigger_slope(self, slope: str) -> None:
        """Set trigger slope (POS, NEG, RFAL)"""
        valid_slopes = ['POS', 'NEG', 'RFAL']
        if slope not in valid_slopes:
            raise ValueError(f"Invalid trigger slope. Must be one of {valid_slopes}")
        self.write(f":TRIG:EDGE:SLOP {slope}")
        logger.debug(f"Trigger slope set to {slope}")

    def force_trigger(self) -> None:
        """Force a trigger event"""
        self.write(":TFOR")
        logger.debug("Trigger forced")

    def run(self) -> None:
        """Start acquisition"""
        self.write(":RUN")
        logger.debug("Acquisition started")

    def stop(self) -> None:
        """Stop acquisition"""
        self.write(":STOP")
        logger.debug("Acquisition stopped")

    def single(self) -> None:
        """Single acquisition"""
        self.write(":SING")
        logger.debug("Single acquisition triggered")

    def get_waveform_data(self, channel: int, points: int = 1000) -> tuple[np.ndarray, np.ndarray]:
        """
        Get waveform data from specified channel

        Args:
            channel: Channel number (1-4)
            points: Number of data points to retrieve

        Returns:
            Tuple of (time_array, voltage_array)
        """
        self.write(f":WAV:SOUR CHAN{channel}")
        self.write(":WAV:FORM ASC")
        self.write(f":WAV:POIN {points}")

        preamble = self.query(":WAV:PRE?").split(',')
        y_increment = float(preamble[7])
        y_origin = float(preamble[8])
        y_reference = float(preamble[9])
        x_increment = float(preamble[4])
        x_origin = float(preamble[5])

        self.write(":WAV:DATA?")
        raw_data = self.inst.read_raw()

        data_str = raw_data.decode('ascii').strip()
        if data_str.startswith('#'):
            header_len = int(data_str[1]) + 2
            data_str = data_str[header_len:]

        data_points = [float(x) for x in data_str.split(',')]
        voltages = [(point - y_reference - y_origin) * y_increment for point in data_points]
        times = [x_origin + i * x_increment for i in range(len(voltages))]

        logger.debug(f"Retrieved {len(voltages)} data points from channel {channel}")
        return np.array(times), np.array(voltages)

    def measure(self, measurement_type: str, channel: int) -> float:
        """
        Make automatic measurement

        Args:
            measurement_type: Type of measurement (FREQ, VPP, VMAX, etc.)
            channel: Channel number (1-4)

        Returns:
            Measurement value as float
        """
        result = self.query(f":MEAS:{measurement_type}? CHAN{channel}")
        value = float(result)
        logger.debug(f"Measurement {measurement_type} on CH{channel}: {value}")
        return value

    def screenshot(self, filename: str = "screenshot.png") -> None:
        """
        Capture screenshot

        Args:
            filename: Output filename for the screenshot
        """
        self.write(":DISP:DATA?")
        img_data = self.inst.read_raw()

        if img_data[0:1] == b'#':
            header_len = int(chr(img_data[1])) + 2
            img_data = img_data[header_len:]

        with open(filename, 'wb') as f:
            f.write(img_data)
        logger.info(f"Screenshot saved to {filename}")

    # Logic Analyzer (Digital Channels) Methods
    def set_digital_display(self, state: bool) -> None:
        """Turn digital channels on/off globally"""
        self.write(f":LA:DISP {1 if state else 0}")
        logger.debug(f"Digital channels display set to {state}")

    def set_digital_channel_display(self, channel: int, state: bool) -> None:
        """
        Turn individual digital channel on/off
        
        Args:
            channel: Digital channel number (0-15)
            state: True to enable, False to disable
        """
        if not 0 <= channel <= 15:
            raise ValueError("Digital channel must be between 0 and 15")
        self.write(f":LA:DIG{channel}:DISP {1 if state else 0}")
        logger.debug(f"Digital channel {channel} display set to {state}")

    def set_digital_threshold(self, threshold_type: str, level: float = None, channel: int = None) -> None:
        """
        Set digital threshold
        
        Args:
            threshold_type: Threshold type - 'TTL', 'CMOS5', 'CMOS3', 'ECL', 'LVTTL', 'LVCMOS3', 'LVCMOS2', 'CUSTOM'
            level: Custom threshold level in volts (only for CUSTOM type)
            channel: Specific channel number (0-15), or None for all channels
        """
        valid_types = ['TTL', 'CMOS5', 'CMOS3', 'ECL', 'LVTTL', 'LVCMOS3', 'LVCMOS2', 'CUSTOM']
        if threshold_type not in valid_types:
            raise ValueError(f"Invalid threshold type. Must be one of {valid_types}")
        
        if channel is not None:
            if not 0 <= channel <= 15:
                raise ValueError("Digital channel must be between 0 and 15")
            if threshold_type == 'CUSTOM' and level is not None:
                self.write(f":LA:DIG{channel}:THR {level}")
                logger.debug(f"Digital channel {channel} custom threshold set to {level} V")
            else:
                self.write(f":LA:DIG{channel}:THR {threshold_type}")
                logger.debug(f"Digital channel {channel} threshold set to {threshold_type}")
        else:
            # Set for all channels
            if threshold_type == 'CUSTOM' and level is not None:
                for ch in range(16):
                    self.write(f":LA:DIG{ch}:THR {level}")
                logger.debug(f"All digital channels custom threshold set to {level} V")
            else:
                self.write(f":LA:THR {threshold_type}")
                logger.debug(f"All digital channels threshold set to {threshold_type}")

    def set_digital_size(self, size: int) -> None:
        """
        Set digital waveform vertical size (height)
        
        Args:
            size: Size value (typically 40-200 pixels)
        """
        self.write(f":LA:SIZE {size}")
        logger.debug(f"Digital waveform size set to {size}")

    def set_digital_position(self, position: int) -> None:
        """
        Set digital waveform vertical position
        
        Args:
            position: Position value
        """
        self.write(f":LA:POS {position}")
        logger.debug(f"Digital waveform position set to {position}")

    def get_digital_data(self, channel: int, points: int = 1000) -> tuple[np.ndarray, np.ndarray]:
        """
        Get digital waveform data from specified channel
        
        Args:
            channel: Digital channel number (0-15)
            points: Number of data points to retrieve
            
        Returns:
            Tuple of (time_array, digital_array) where digital_array contains 0 or 1
        """
        if not 0 <= channel <= 15:
            raise ValueError("Digital channel must be between 0 and 15")
            
        self.write(f":WAV:SOUR DIG{channel}")
        self.write(":WAV:FORM ASC")
        self.write(f":WAV:POIN {points}")

        preamble = self.query(":WAV:PRE?").split(',')
        x_increment = float(preamble[4])
        x_origin = float(preamble[5])

        self.write(":WAV:DATA?")
        raw_data = self.inst.read_raw()

        data_str = raw_data.decode('ascii').strip()
        if data_str.startswith('#'):
            header_len = int(data_str[1]) + 2
            data_str = data_str[header_len:]

        data_points = [int(float(x)) for x in data_str.split(',')]
        times = [x_origin + i * x_increment for i in range(len(data_points))]

        logger.debug(f"Retrieved {len(data_points)} digital data points from channel {channel}")
        return np.array(times), np.array(data_points)

    def set_digital_label(self, channel: int, label: str) -> None:
        """
        Set label for digital channel
        
        Args:
            channel: Digital channel number (0-15)
            label: Label string (max 4 characters)
        """
        if not 0 <= channel <= 15:
            raise ValueError("Digital channel must be between 0 and 15")
        self.write(f':LA:DIG{channel}:LAB "{label[:4]}"')
        logger.debug(f"Digital channel {channel} label set to '{label[:4]}'")

    def get_digital_label(self, channel: int) -> str:
        """
        Get label for digital channel
        
        Args:
            channel: Digital channel number (0-15)
            
        Returns:
            Label string
        """
        if not 0 <= channel <= 15:
            raise ValueError("Digital channel must be between 0 and 15")
        label = self.query(f":LA:DIG{channel}:LAB?")
        return label.strip('"')

    def close(self) -> None:
        """Close connection"""
        logger.info("Closing oscilloscope connection")
        self.inst.close()
        self.rm.close()