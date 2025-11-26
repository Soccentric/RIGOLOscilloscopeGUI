"""
Configuration management for RIGOL Oscilloscope GUI

Author: Sandesh Ghimire <sandesh@soccentric.com>
"""

import json
import os
from typing import Dict, Any, Optional
from pathlib import Path


class Config:
    """Configuration manager for the oscilloscope GUI"""

    DEFAULT_CONFIG = {
        "instrument": {
            "timeout": 10000,
            "auto_detect": True,
            "resource_string": None
        },
        "gui": {
            "window_size": "1400x900",
            "theme": "dark",
            "auto_update_rate": 2.0,
            "default_points": 1000
        },
        "channels": {
            "default_scale": 1.0,
            "default_offset": 0.0,
            "default_coupling": "DC",
            "default_probe": 10.0
        },
        "logic_analyzer": {
            "enabled": False,
            "default_threshold": "TTL",
            "custom_threshold_level": 1.5,
            "default_size": 100,
            "default_position": 0,
            "default_labels": {
                "D0": "D0", "D1": "D1", "D2": "D2", "D3": "D3",
                "D4": "D4", "D5": "D5", "D6": "D6", "D7": "D7",
                "D8": "D8", "D9": "D9", "D10": "D10", "D11": "D11",
                "D12": "D12", "D13": "D13", "D14": "D14", "D15": "D15"
            }
        },
        "timebase": {
            "default_scale": 1e-3,
            "default_offset": 0.0
        },
        "trigger": {
            "default_mode": "AUTO",
            "default_source": "CHAN1",
            "default_level": 0.0,
            "default_slope": "POS"
        },
        "logging": {
            "level": "INFO",
            "file": "rigol_gui.log"
        }
    }

    def __init__(self, config_file: str = "config.json"):
        """
        Initialize configuration manager

        Args:
            config_file: Path to configuration file
        """
        self.config_file = Path(config_file)
        self.config = self.DEFAULT_CONFIG.copy()
        self.load()

    def load(self) -> None:
        """Load configuration from file"""
        if self.config_file.exists():
            try:
                with open(self.config_file, 'r') as f:
                    loaded_config = json.load(f)
                    self._merge_config(self.config, loaded_config)
                print(f"Configuration loaded from {self.config_file}")
            except (json.JSONDecodeError, IOError) as e:
                print(f"Warning: Could not load config file {self.config_file}: {e}")
                print("Using default configuration")

    def save(self) -> None:
        """Save current configuration to file"""
        try:
            # Create directory if it doesn't exist
            self.config_file.parent.mkdir(parents=True, exist_ok=True)

            with open(self.config_file, 'w') as f:
                json.dump(self.config, f, indent=2)
            print(f"Configuration saved to {self.config_file}")
        except IOError as e:
            print(f"Warning: Could not save config file {self.config_file}: {e}")

    def _merge_config(self, base: Dict[str, Any], update: Dict[str, Any]) -> None:
        """Recursively merge update dict into base dict"""
        for key, value in update.items():
            if key in base and isinstance(base[key], dict) and isinstance(value, dict):
                self._merge_config(base[key], value)
            else:
                base[key] = value

    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value by dot-separated key

        Args:
            key: Dot-separated key (e.g., 'gui.window_size')
            default: Default value if key not found

        Returns:
            Configuration value
        """
        keys = key.split('.')
        value = self.config

        for k in keys:
            if isinstance(value, dict) and k in value:
                value = value[k]
            else:
                return default

        return value

    def set(self, key: str, value: Any) -> None:
        """
        Set configuration value by dot-separated key

        Args:
            key: Dot-separated key (e.g., 'gui.window_size')
            value: Value to set
        """
        keys = key.split('.')
        config = self.config

        # Navigate to the parent dict
        for k in keys[:-1]:
            if k not in config:
                config[k] = {}
            config = config[k]

        # Set the value
        config[keys[-1]] = value

    def reset_to_defaults(self) -> None:
        """Reset configuration to default values"""
        self.config = self.DEFAULT_CONFIG.copy()