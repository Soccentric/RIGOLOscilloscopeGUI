"""
Utility functions for RIGOL Oscilloscope GUI

Author: Sandesh Ghimire <sandesh@soccentric.com>
"""

import logging
from typing import Optional


def setup_logging(level: str = "INFO", log_file: Optional[str] = None) -> None:
    """
    Setup logging configuration

    Args:
        level: Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
        log_file: Optional log file path
    """
    # Convert string level to logging level
    numeric_level = getattr(logging, level.upper(), logging.INFO)

    # Configure logging
    logging.basicConfig(
        level=numeric_level,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[
            logging.StreamHandler(),
            *(logging.FileHandler(log_file) for _ in [log_file] if log_file)
        ]
    )


def format_measurement_value(value: float, measurement_type: str) -> str:
    """
    Format measurement values with appropriate units

    Args:
        value: Raw measurement value
        measurement_type: Type of measurement

    Returns:
        Formatted string with units
    """
    if measurement_type == 'FREQ':
        if value >= 1e6:
            return f"{value/1e6:.3f} MHz"
        elif value >= 1e3:
            return f"{value/1e3:.3f} kHz"
        else:
            return f"{value:.3f} Hz"
    elif measurement_type in ['PER', 'PWID']:
        if value >= 1:
            return f"{value:.3f} s"
        elif value >= 1e-3:
            return f"{value*1e3:.3f} ms"
        elif value >= 1e-6:
            return f"{value*1e6:.3f} µs"
        else:
            return f"{value*1e9:.3f} ns"
    else:  # Voltage measurements
        if abs(value) >= 1:
            return f"{value:.3f} V"
        elif abs(value) >= 1e-3:
            return f"{value*1e3:.3f} mV"
        else:
            return f"{value*1e6:.3f} µV"


def validate_channel_number(channel: int) -> bool:
    """Validate channel number (1-4)"""
    return 1 <= channel <= 4


def validate_scale_value(scale: float) -> bool:
    """Validate scale value (reasonable range)"""
    return 1e-9 <= scale <= 1e3


def validate_offset_value(offset: float) -> bool:
    """Validate offset value (reasonable range)"""
    return -1e3 <= offset <= 1e3


def validate_trigger_level(level: float) -> bool:
    """Validate trigger level (reasonable range)"""
    return -100 <= level <= 100


def validate_digital_channel(channel: int) -> bool:
    """
    Validate digital channel number (0-15)
    
    Args:
        channel: Digital channel number
        
    Returns:
        True if valid, False otherwise
    """
    return 0 <= channel <= 15


def validate_digital_threshold(threshold: float) -> bool:
    """
    Validate digital threshold voltage
    
    Args:
        threshold: Threshold voltage in volts
        
    Returns:
        True if valid, False otherwise
    """
    return -10 <= threshold <= 10


def validate_digital_label(label: str) -> bool:
    """
    Validate digital channel label
    
    Args:
        label: Label string
        
    Returns:
        True if valid, False otherwise
    """
    return len(label) <= 4 and len(label) > 0


def is_valid_threshold_type(threshold_type: str) -> bool:
    """
    Check if threshold type is valid
    
    Args:
        threshold_type: Threshold type string
        
    Returns:
        True if valid, False otherwise
    """
    valid_types = ['TTL', 'CMOS5', 'CMOS3', 'ECL', 'LVTTL', 'LVCMOS3', 'LVCMOS2', 'CUSTOM']
    return threshold_type in valid_types


def get_threshold_voltage(threshold_type: str) -> float:
    """
    Get the threshold voltage for standard threshold types
    
    Args:
        threshold_type: Threshold type string
        
    Returns:
        Threshold voltage in volts
    """
    thresholds = {
        'TTL': 1.4,
        'CMOS5': 2.5,
        'CMOS3': 1.65,
        'ECL': -1.3,
        'LVTTL': 1.4,
        'LVCMOS3': 1.65,
        'LVCMOS2': 1.25
    }
    return thresholds.get(threshold_type, 1.5)
