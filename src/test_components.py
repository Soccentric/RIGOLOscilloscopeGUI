#!/usr/bin/env python3
"""
Basic tests for RIGOL Oscilloscope GUI components
Run with: python test_components.py

Author: Sandesh Ghimire <sandesh@soccentric.com>
"""

import sys
import os

# Add current directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_config():
    """Test configuration management"""
    print("Testing configuration...")
    from config import Config

    config = Config(":memory:")  # Use in-memory config for testing

    # Test setting and getting values
    config.set('test.value', 'hello')
    assert config.get('test.value') == 'hello'

    config.set('test.number', 42)
    assert config.get('test.number') == 42

    print("✓ Configuration tests passed")

def test_utils():
    """Test utility functions"""
    print("Testing utilities...")
    from utils import (format_measurement_value, validate_scale_value, validate_channel_number,
                      validate_digital_channel, validate_digital_threshold, validate_digital_label,
                      is_valid_threshold_type, get_threshold_voltage)

    # Test measurement formatting
    assert format_measurement_value(1000.0, 'FREQ') == "1.000 kHz"
    assert format_measurement_value(0.001, 'VPP') == "1.000 mV"
    assert format_measurement_value(0.000001, 'PER') == "1.000 µs"

    # Test analog channel validation
    assert validate_scale_value(0.001) == True
    assert validate_scale_value(1000) == True
    assert validate_scale_value(1e-10) == False  # Too small
    assert validate_scale_value(1e5) == False    # Too large

    assert validate_channel_number(1) == True
    assert validate_channel_number(4) == True
    assert validate_channel_number(0) == False
    assert validate_channel_number(5) == False

    # Test digital channel validation
    assert validate_digital_channel(0) == True
    assert validate_digital_channel(15) == True
    assert validate_digital_channel(-1) == False
    assert validate_digital_channel(16) == False

    # Test digital threshold validation
    assert validate_digital_threshold(1.5) == True
    assert validate_digital_threshold(-5.0) == True
    assert validate_digital_threshold(10.0) == True
    assert validate_digital_threshold(10.1) == False
    assert validate_digital_threshold(-10.1) == False

    # Test digital label validation
    assert validate_digital_label("CLK") == True
    assert validate_digital_label("D0") == True
    assert validate_digital_label("ABCD") == True
    assert validate_digital_label("ABCDE") == False  # Too long
    assert validate_digital_label("") == False       # Empty

    # Test threshold type validation
    assert is_valid_threshold_type("TTL") == True
    assert is_valid_threshold_type("CMOS5") == True
    assert is_valid_threshold_type("CUSTOM") == True
    assert is_valid_threshold_type("INVALID") == False

    # Test threshold voltage retrieval
    assert get_threshold_voltage("TTL") == 1.4
    assert get_threshold_voltage("CMOS5") == 2.5
    assert get_threshold_voltage("CMOS3") == 1.65
    assert get_threshold_voltage("INVALID") == 1.5  # Default

    print("✓ Utility tests passed (including logic analyzer functions)")

def test_imports():
    """Test that all modules can be imported"""
    print("Testing imports...")
    try:
        import rigol_instrument
        import config
        import utils
        import rigol_gui
        print("✓ All modules imported successfully")
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False
    return True

def main():
    """Run all tests"""
    print("Running RIGOL GUI component tests...\n")

    try:
        test_imports()
        test_config()
        test_utils()

        print("\n✓ All tests passed! The RIGOL GUI components are working correctly.")
        return 0

    except Exception as e:
        print(f"\n✗ Test failed: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())