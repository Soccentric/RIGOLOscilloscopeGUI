"""
RIGOL DHO954 Oscilloscope Full-Featured GUI Interface
Main GUI application module

Author: Sandesh Ghimire <sandesh@soccentric.com>
"""

import time
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import threading
import logging

from rigol_instrument import RigolDHO954
from config import Config
from utils import setup_logging, format_measurement_value, validate_channel_number, validate_scale_value, validate_offset_value, validate_trigger_level

logger = logging.getLogger(__name__)


class OscilloscopeGUI:
    """Main GUI application for RIGOL DHO954 oscilloscope control"""

    def __init__(self, root: tk.Tk, config: Config):
        """
        Initialize the GUI application

        Args:
            root: Tkinter root window
            config: Configuration object
        """
        self.root = root
        self.config = config
        self.scope: Optional[RigolDHO954] = None
        self.is_running = False
        self.update_thread: Optional[threading.Thread] = None

        # Load configuration values
        self.window_size = self.config.get('gui.window_size', '1400x900')
        self.auto_update_rate = self.config.get('gui.auto_update_rate', 2.0)
        self.default_points = self.config.get('gui.default_points', 1000)

        self.setup_ui()

        logger.info("Oscilloscope GUI initialized")

    def setup_ui(self) -> None:
        """Setup the complete UI"""
        self.root.title("RIGOL DHO954 Oscilloscope Control")
        self.root.geometry(self.window_size)

        # Configure styles
        style = ttk.Style()
        style.theme_use('clam')

        # Custom button styles
        style.configure('Green.TButton', foreground='green')
        style.configure('Red.TButton', foreground='red')

        # Top toolbar
        toolbar = ttk.Frame(self.root)
        toolbar.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)

        ttk.Button(toolbar, text="Connect", command=self.connect_scope).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Disconnect", command=self.disconnect_scope).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Reset", command=self.reset_scope).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Autoscale", command=self.autoscale).pack(side=tk.LEFT, padx=2)

        ttk.Separator(toolbar, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=5)

        ttk.Button(toolbar, text="RUN", command=self.run_scope, style="Green.TButton").pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="STOP", command=self.stop_scope, style="Red.TButton").pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="SINGLE", command=self.single_scope).pack(side=tk.LEFT, padx=2)

        ttk.Separator(toolbar, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=5)

        ttk.Button(toolbar, text="Screenshot", command=self.take_screenshot).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Save Waveform", command=self.save_waveform).pack(side=tk.LEFT, padx=2)

        self.status_label = ttk.Label(toolbar, text="Not Connected", foreground="red")
        self.status_label.pack(side=tk.RIGHT, padx=10)

        # Main container
        main_container = ttk.Frame(self.root)
        main_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Left panel - Controls
        left_panel = ttk.Frame(main_container, width=350)
        left_panel.pack(side=tk.LEFT, fill=tk.Y, padx=(0, 5))
        left_panel.pack_propagate(False)

        # Right panel - Waveform display and measurements
        right_panel = ttk.Frame(main_container)
        right_panel.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        # Setup left panel sections
        self.setup_channel_controls(left_panel)
        self.setup_timebase_controls(left_panel)
        self.setup_trigger_controls(left_panel)
        self.setup_acquisition_controls(left_panel)

        # Setup right panel
        self.setup_waveform_display(right_panel)
        self.setup_measurements_panel(right_panel)

    def setup_channel_controls(self, parent: ttk.Frame) -> None:
        """Setup channel control section"""
        frame = ttk.LabelFrame(parent, text="Channel Controls", padding=10)
        frame.pack(fill=tk.X, pady=5)

        self.channel_vars = {}
        self.channel_frames = {}

        for ch in range(1, 5):
            ch_frame = ttk.Frame(frame)
            ch_frame.pack(fill=tk.X, pady=5)

            # Channel enable
            var = tk.BooleanVar(value=True if ch == 1 else False)
            self.channel_vars[ch] = var
            chk = ttk.Checkbutton(ch_frame, text=f"CH{ch}", variable=var,
                                  command=lambda c=ch: self.update_channel_display(c))
            chk.pack(side=tk.LEFT)

            # Scale
            ttk.Label(ch_frame, text="V/div:").pack(side=tk.LEFT, padx=(10, 2))
            scale_var = tk.StringVar(value=str(self.config.get('channels.default_scale', 1.0)))
            self.channel_vars[f'ch{ch}_scale'] = scale_var
            scale_combo = ttk.Combobox(ch_frame, textvariable=scale_var, width=8,
                                       values=['0.001', '0.002', '0.005', '0.01', '0.02', '0.05',
                                               '0.1', '0.2', '0.5', '1.0', '2.0', '5.0', '10.0'])
            scale_combo.pack(side=tk.LEFT, padx=2)
            scale_combo.bind('<<ComboboxSelected>>', lambda e, c=ch: self.update_channel_scale(c))

            # Offset
            ttk.Label(ch_frame, text="Offset:").pack(side=tk.LEFT, padx=(10, 2))
            offset_var = tk.StringVar(value=str(self.config.get('channels.default_offset', 0.0)))
            self.channel_vars[f'ch{ch}_offset'] = offset_var
            offset_entry = ttk.Entry(ch_frame, textvariable=offset_var, width=6)
            offset_entry.pack(side=tk.LEFT, padx=2)
            offset_entry.bind('<Return>', lambda e, c=ch: self.update_channel_offset(c))

            # Coupling
            ch_frame2 = ttk.Frame(frame)
            ch_frame2.pack(fill=tk.X, pady=2)

            ttk.Label(ch_frame2, text=f"  CH{ch} Coupling:").pack(side=tk.LEFT, padx=(0, 5))
            coupling_var = tk.StringVar(value=self.config.get('channels.default_coupling', 'DC'))
            self.channel_vars[f'ch{ch}_coupling'] = coupling_var
            for coup in ['DC', 'AC', 'GND']:
                ttk.Radiobutton(ch_frame2, text=coup, variable=coupling_var, value=coup,
                                command=lambda c=ch: self.update_channel_coupling(c)).pack(side=tk.LEFT, padx=2)

            # Probe
            ttk.Label(ch_frame2, text="Probe:").pack(side=tk.LEFT, padx=(10, 2))
            probe_var = tk.StringVar(value=str(self.config.get('channels.default_probe', 10.0)))
            self.channel_vars[f'ch{ch}_probe'] = probe_var
            probe_combo = ttk.Combobox(ch_frame2, textvariable=probe_var, width=6,
                                       values=['0.01', '0.1', '1', '10', '100', '1000'])
            probe_combo.pack(side=tk.LEFT, padx=2)
            probe_combo.bind('<<ComboboxSelected>>', lambda e, c=ch: self.update_channel_probe(c))

    def setup_timebase_controls(self, parent: ttk.Frame) -> None:
        """Setup timebase control section"""
        frame = ttk.LabelFrame(parent, text="Timebase", padding=10)
        frame.pack(fill=tk.X, pady=5)

        # Time/div
        ttk.Label(frame, text="Time/div:").pack(side=tk.LEFT, padx=5)
        self.timebase_scale_var = tk.StringVar(value=str(self.config.get('timebase.default_scale', 1e-3)))
        timebase_combo = ttk.Combobox(frame, textvariable=self.timebase_scale_var, width=10,
                                      values=['1e-9', '2e-9', '5e-9', '1e-8', '2e-8', '5e-8',
                                              '1e-7', '2e-7', '5e-7', '1e-6', '2e-6', '5e-6',
                                              '1e-5', '2e-5', '5e-5', '1e-4', '2e-4', '5e-4',
                                              '1e-3', '2e-3', '5e-3', '1e-2', '2e-2', '5e-2',
                                              '1e-1', '2e-1', '5e-1', '1', '2', '5', '10'])
        timebase_combo.pack(side=tk.LEFT, padx=5)
        timebase_combo.bind('<<ComboboxSelected>>', lambda e: self.update_timebase())

        # Offset
        ttk.Label(frame, text="Offset:").pack(side=tk.LEFT, padx=(20, 5))
        self.timebase_offset_var = tk.StringVar(value=str(self.config.get('timebase.default_offset', 0.0)))
        offset_entry = ttk.Entry(frame, textvariable=self.timebase_offset_var, width=10)
        offset_entry.pack(side=tk.LEFT, padx=5)
        offset_entry.bind('<Return>', lambda e: self.update_timebase())

    def setup_trigger_controls(self, parent: ttk.Frame) -> None:
        """Setup trigger control section"""
        frame = ttk.LabelFrame(parent, text="Trigger", padding=10)
        frame.pack(fill=tk.X, pady=5)

        # Mode
        mode_frame = ttk.Frame(frame)
        mode_frame.pack(fill=tk.X, pady=5)
        ttk.Label(mode_frame, text="Mode:").pack(side=tk.LEFT, padx=5)
        self.trigger_mode_var = tk.StringVar(value=self.config.get('trigger.default_mode', 'AUTO'))
        for mode in ['AUTO', 'NORM', 'SING']:
            ttk.Radiobutton(mode_frame, text=mode, variable=self.trigger_mode_var, value=mode,
                            command=self.update_trigger).pack(side=tk.LEFT, padx=5)

        # Source
        source_frame = ttk.Frame(frame)
        source_frame.pack(fill=tk.X, pady=5)
        ttk.Label(source_frame, text="Source:").pack(side=tk.LEFT, padx=5)
        self.trigger_source_var = tk.StringVar(value=self.config.get('trigger.default_source', 'CHAN1'))
        source_combo = ttk.Combobox(source_frame, textvariable=self.trigger_source_var, width=10,
                                    values=['CHAN1', 'CHAN2', 'CHAN3', 'CHAN4', 'EXT', 'LINE'])
        source_combo.pack(side=tk.LEFT, padx=5)
        source_combo.bind('<<ComboboxSelected>>', lambda e: self.update_trigger())

        # Level
        level_frame = ttk.Frame(frame)
        level_frame.pack(fill=tk.X, pady=5)
        ttk.Label(level_frame, text="Level (V):").pack(side=tk.LEFT, padx=5)
        self.trigger_level_var = tk.StringVar(value=str(self.config.get('trigger.default_level', 0.0)))
        level_entry = ttk.Entry(level_frame, textvariable=self.trigger_level_var, width=10)
        level_entry.pack(side=tk.LEFT, padx=5)
        level_entry.bind('<Return>', lambda e: self.update_trigger())

        # Slope
        slope_frame = ttk.Frame(frame)
        slope_frame.pack(fill=tk.X, pady=5)
        ttk.Label(slope_frame, text="Slope:").pack(side=tk.LEFT, padx=5)
        self.trigger_slope_var = tk.StringVar(value=self.config.get('trigger.default_slope', 'POS'))
        for slope in ['POS', 'NEG', 'RFAL']:
            ttk.Radiobutton(slope_frame, text=slope, variable=self.trigger_slope_var, value=slope,
                            command=self.update_trigger).pack(side=tk.LEFT, padx=5)

        # Force trigger button
        ttk.Button(frame, text="Force Trigger", command=self.force_trigger).pack(pady=5)

    def setup_acquisition_controls(self, parent: ttk.Frame) -> None:
        """Setup acquisition control section"""
        frame = ttk.LabelFrame(parent, text="Acquisition", padding=10)
        frame.pack(fill=tk.X, pady=5)

        # Update rate
        rate_frame = ttk.Frame(frame)
        rate_frame.pack(fill=tk.X, pady=5)
        ttk.Label(rate_frame, text="Update Rate (Hz):").pack(side=tk.LEFT, padx=5)
        self.update_rate_var = tk.StringVar(value=str(self.auto_update_rate))
        rate_combo = ttk.Combobox(rate_frame, textvariable=self.update_rate_var, width=8,
                                  values=['0.5', '1', '2', '5', '10', '20'])
        rate_combo.pack(side=tk.LEFT, padx=5)

        # Number of points
        points_frame = ttk.Frame(frame)
        points_frame.pack(fill=tk.X, pady=5)
        ttk.Label(points_frame, text="Points:").pack(side=tk.LEFT, padx=5)
        self.points_var = tk.StringVar(value=str(self.default_points))
        points_combo = ttk.Combobox(points_frame, textvariable=self.points_var, width=8,
                                    values=['500', '1000', '2000', '5000', '10000'])
        points_combo.pack(side=tk.LEFT, padx=5)

        # Auto update checkbox
        self.auto_update_var = tk.BooleanVar(value=False)
        ttk.Checkbutton(frame, text="Auto Update Waveform",
                        variable=self.auto_update_var,
                        command=self.toggle_auto_update).pack(pady=5)

        # Manual update button
        ttk.Button(frame, text="Update Waveform Now",
                   command=self.update_waveform).pack(pady=5)

    def setup_waveform_display(self, parent: ttk.Frame) -> None:
        """Setup waveform display area"""
        frame = ttk.LabelFrame(parent, text="Waveform Display", padding=5)
        frame.pack(fill=tk.BOTH, expand=True, pady=(0, 5))

        # Create matplotlib figure
        self.fig = Figure(figsize=(8, 6), dpi=100, facecolor='black')
        self.ax = self.fig.add_subplot(111, facecolor='#001a00')

        # Style the plot like an oscilloscope
        self.ax.grid(True, color='#003300', linestyle='-', linewidth=0.5)
        self.ax.set_xlabel('Time', color='white')
        self.ax.set_ylabel('Voltage (V)', color='white')
        self.ax.tick_params(colors='white')
        self.ax.spines['bottom'].set_color('white')
        self.ax.spines['top'].set_color('white')
        self.ax.spines['left'].set_color('white')
        self.ax.spines['right'].set_color('white')

        self.fig.tight_layout()

        # Create canvas
        self.canvas = FigureCanvasTkAgg(self.fig, master=frame)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        # Store line objects for each channel
        self.waveform_lines = {}
        colors = ['#00ff00', '#ffff00', '#00ffff', '#ff00ff']
        for i in range(1, 5):
            line, = self.ax.plot([], [], color=colors[i-1], linewidth=1.5, label=f'CH{i}')
            self.waveform_lines[i] = line

        self.ax.legend(loc='upper right', facecolor='black', edgecolor='white',
                       labelcolor='white', fontsize=8)

    def setup_measurements_panel(self, parent: ttk.Frame) -> None:
        """Setup measurements display panel"""
        frame = ttk.LabelFrame(parent, text="Measurements", padding=10)
        frame.pack(fill=tk.X, pady=(0, 0))

        # Create measurement displays for each channel
        self.measurement_vars = {}

        for ch in range(1, 5):
            ch_frame = ttk.LabelFrame(frame, text=f"Channel {ch}")
            ch_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5)

            measurements = ['FREQ', 'VPP', 'VMAX', 'VMIN', 'VRMS', 'VAVG', 'PER', 'PWID']

            for meas in measurements:
                meas_frame = ttk.Frame(ch_frame)
                meas_frame.pack(fill=tk.X, pady=2)

                ttk.Label(meas_frame, text=f"{meas}:", width=6).pack(side=tk.LEFT)
                var = tk.StringVar(value="---")
                self.measurement_vars[f'ch{ch}_{meas}'] = var
                ttk.Label(meas_frame, textvariable=var, width=12,
                          relief=tk.SUNKEN).pack(side=tk.LEFT, fill=tk.X, expand=True)

        # Update measurements button
        ttk.Button(frame, text="Update Measurements",
                   command=self.update_measurements).pack(pady=5)

    # Connection methods
    def connect_scope(self) -> None:
        """Connect to the oscilloscope"""
        try:
            resource_string = self.config.get('instrument.resource_string')
            timeout = self.config.get('instrument.timeout', 10000)
            self.scope = RigolDHO954(resource_string=resource_string, timeout=timeout)
            self.status_label.config(text=f"Connected: {self.scope.idn}", foreground="green")
            messagebox.showinfo("Success", f"Connected to:\n{self.scope.idn}")
            logger.info("Successfully connected to oscilloscope")
        except Exception as e:
            error_msg = f"Connection Error: {str(e)}"
            messagebox.showerror("Connection Error", error_msg)
            self.status_label.config(text="Connection Failed", foreground="red")
            logger.error(error_msg)

    def disconnect_scope(self) -> None:
        """Disconnect from the oscilloscope"""
        if self.scope:
            self.is_running = False
            time.sleep(0.5)
            self.scope.close()
            self.scope = None
            self.status_label.config(text="Disconnected", foreground="red")
            logger.info("Disconnected from oscilloscope")

    # Control methods
    def reset_scope(self) -> None:
        """Reset the oscilloscope"""
        if not self.scope:
            messagebox.showwarning("Warning", "Not connected to oscilloscope")
            return
        try:
            self.scope.reset()
            messagebox.showinfo("Success", "Oscilloscope reset")
            logger.info("Oscilloscope reset completed")
        except Exception as e:
            error_msg = f"Reset error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def autoscale(self) -> None:
        """Perform autoscale"""
        if not self.scope:
            messagebox.showwarning("Warning", "Not connected to oscilloscope")
            return
        try:
            self.scope.autoscale()
            messagebox.showinfo("Success", "Autoscale complete")
            logger.info("Autoscale completed")
        except Exception as e:
            error_msg = f"Autoscale error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def run_scope(self) -> None:
        """Start acquisition"""
        if not self.scope:
            messagebox.showwarning("Warning", "Not connected to oscilloscope")
            return
        try:
            self.scope.run()
            self.status_label.config(text="Running", foreground="green")
            logger.debug("Acquisition started")
        except Exception as e:
            error_msg = f"Run error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def stop_scope(self) -> None:
        """Stop acquisition"""
        if not self.scope:
            messagebox.showwarning("Warning", "Not connected to oscilloscope")
            return
        try:
            self.scope.stop()
            self.status_label.config(text="Stopped", foreground="orange")
            logger.debug("Acquisition stopped")
        except Exception as e:
            error_msg = f"Stop error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def single_scope(self) -> None:
        """Single acquisition"""
        if not self.scope:
            messagebox.showwarning("Warning", "Not connected to oscilloscope")
            return
        try:
            self.scope.single()
            logger.debug("Single acquisition triggered")
        except Exception as e:
            error_msg = f"Single acquisition error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    # Update methods for controls
    def update_channel_display(self, channel: int) -> None:
        """Update channel display state"""
        if not self.scope:
            return
        try:
            state = self.channel_vars[channel].get()
            self.scope.set_channel_display(channel, state)
            self.waveform_lines[channel].set_visible(state)
            self.canvas.draw()
            logger.debug(f"Channel {channel} display updated to {state}")
        except Exception as e:
            error_msg = f"Channel display update error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def update_channel_scale(self, channel: int) -> None:
        """Update channel scale"""
        if not self.scope:
            return
        try:
            scale_str = self.channel_vars[f'ch{channel}_scale'].get()
            scale = float(scale_str)
            if not validate_scale_value(scale):
                raise ValueError(f"Invalid scale value: {scale}")
            self.scope.set_channel_scale(channel, scale)
            logger.debug(f"Channel {channel} scale updated to {scale}")
        except Exception as e:
            error_msg = f"Channel scale update error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def update_channel_offset(self, channel: int) -> None:
        """Update channel offset"""
        if not self.scope:
            return
        try:
            offset_str = self.channel_vars[f'ch{channel}_offset'].get()
            offset = float(offset_str)
            if not validate_offset_value(offset):
                raise ValueError(f"Invalid offset value: {offset}")
            self.scope.set_channel_offset(channel, offset)
            logger.debug(f"Channel {channel} offset updated to {offset}")
        except Exception as e:
            error_msg = f"Channel offset update error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def update_channel_coupling(self, channel: int) -> None:
        """Update channel coupling"""
        if not self.scope:
            return
        try:
            coupling = self.channel_vars[f'ch{channel}_coupling'].get()
            self.scope.set_channel_coupling(channel, coupling)
            logger.debug(f"Channel {channel} coupling updated to {coupling}")
        except Exception as e:
            error_msg = f"Channel coupling update error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def update_channel_probe(self, channel: int) -> None:
        """Update probe ratio"""
        if not self.scope:
            return
        try:
            probe_str = self.channel_vars[f'ch{channel}_probe'].get()
            ratio = float(probe_str)
            self.scope.set_channel_probe(channel, ratio)
            logger.debug(f"Channel {channel} probe ratio updated to {ratio}")
        except Exception as e:
            error_msg = f"Channel probe update error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def update_timebase(self) -> None:
        """Update timebase settings"""
        if not self.scope:
            return
        try:
            scale = float(self.timebase_scale_var.get())
            offset = float(self.timebase_offset_var.get())
            self.scope.set_timebase_scale(scale)
            self.scope.set_timebase_offset(offset)
            logger.debug(f"Timebase updated: scale={scale}, offset={offset}")
        except Exception as e:
            error_msg = f"Timebase update error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def update_trigger(self) -> None:
        """Update trigger settings"""
        if not self.scope:
            return
        try:
            mode = self.trigger_mode_var.get()
            source = self.trigger_source_var.get()
            level_str = self.trigger_level_var.get()
            level = float(level_str)
            if not validate_trigger_level(level):
                raise ValueError(f"Invalid trigger level: {level}")
            slope = self.trigger_slope_var.get()

            self.scope.set_trigger_mode(mode)
            self.scope.set_trigger_source(source)
            self.scope.set_trigger_level(level)
            self.scope.set_trigger_slope(slope)
            logger.debug(f"Trigger updated: mode={mode}, source={source}, level={level}, slope={slope}")
        except Exception as e:
            error_msg = f"Trigger update error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def force_trigger(self) -> None:
        """Force trigger"""
        if not self.scope:
            return
        try:
            self.scope.force_trigger()
            logger.debug("Trigger forced")
        except Exception as e:
            error_msg = f"Force trigger error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    # Waveform update methods
    def toggle_auto_update(self) -> None:
        """Toggle automatic waveform updates"""
        if self.auto_update_var.get():
            self.is_running = True
            self.update_thread = threading.Thread(target=self.auto_update_loop, daemon=True)
            self.update_thread.start()
            logger.info("Auto-update started")
        else:
            self.is_running = False
            logger.info("Auto-update stopped")

    def auto_update_loop(self) -> None:
        """Auto update loop running in separate thread"""
        while self.is_running:
            try:
                self.update_waveform()
                rate = float(self.update_rate_var.get())
                time.sleep(1.0 / rate)
            except Exception as e:
                logger.error(f"Auto update error: {e}")
                time.sleep(1)

    def update_waveform(self) -> None:
        """Update waveform display"""
        if not self.scope:
            return

        try:
            points = int(self.points_var.get())

            # Get data for each enabled channel
            for ch in range(1, 5):
                if self.channel_vars[ch].get():
                    try:
                        time_data, voltage_data = self.scope.get_waveform_data(ch, points)
                        self.waveform_lines[ch].set_data(time_data, voltage_data)
                    except Exception as e:
                        logger.error(f"Error reading channel {ch}: {e}")

            # Auto-scale axes
            self.ax.relim()
            self.ax.autoscale_view()

            # Redraw canvas
            self.canvas.draw()

            logger.debug("Waveform display updated")

        except Exception as e:
            logger.error(f"Waveform update error: {e}")

    def update_measurements(self) -> None:
        """Update all measurements"""
        if not self.scope:
            messagebox.showwarning("Warning", "Not connected to oscilloscope")
            return

        measurements = ['FREQ', 'VPP', 'VMAX', 'VMIN', 'VRMS', 'VAVG', 'PER', 'PWID']

        for ch in range(1, 5):
            if not self.channel_vars[ch].get():
                continue

            for meas in measurements:
                try:
                    value = self.scope.measure(meas, ch)
                    formatted_value = format_measurement_value(value, meas)
                    self.measurement_vars[f'ch{ch}_{meas}'].set(formatted_value)
                except Exception as e:
                    self.measurement_vars[f'ch{ch}_{meas}'].set("Error")
                    logger.error(f"Measurement error CH{ch} {meas}: {e}")

        logger.debug("Measurements updated")

    def take_screenshot(self) -> None:
        """Take oscilloscope screenshot"""
        if not self.scope:
            messagebox.showwarning("Warning", "Not connected to oscilloscope")
            return

        filename = filedialog.asksaveasfilename(
            defaultextension=".png",
            filetypes=[("PNG files", "*.png"), ("All files", "*.*")]
        )

        if filename:
            try:
                self.scope.screenshot(filename)
                messagebox.showinfo("Success", f"Screenshot saved to {filename}")
                logger.info(f"Screenshot saved to {filename}")
            except Exception as e:
                error_msg = f"Screenshot error: {str(e)}"
                messagebox.showerror("Error", error_msg)
                logger.error(error_msg)

    def save_waveform(self) -> None:
        """Save waveform data to CSV"""
        if not self.scope:
            messagebox.showwarning("Warning", "Not connected to oscilloscope")
            return

        filename = filedialog.asksaveasfilename(
            defaultextension=".csv",
            filetypes=[("CSV files", "*.csv"), ("All files", "*.*")]
        )

        if filename:
            try:
                points = int(self.points_var.get())

                with open(filename, 'w') as f:
                    # Write header
                    header = "Time"
                    for ch in range(1, 5):
                        if self.channel_vars[ch].get():
                            header += f",CH{ch}"
                    f.write(header + "\n")

                    # Get data for enabled channels
                    data = {}
                    time_data = None

                    for ch in range(1, 5):
                        if self.channel_vars[ch].get():
                            time_data, voltage_data = self.scope.get_waveform_data(ch, points)
                            data[ch] = voltage_data

                    # Write data
                    if time_data is not None:
                        for i in range(len(time_data)):
                            line = f"{time_data[i]}"
                            for ch in range(1, 5):
                                if ch in data:
                                    line += f",{data[ch][i]}"
                            f.write(line + "\n")

                messagebox.showinfo("Success", f"Waveform data saved to {filename}")
                logger.info(f"Waveform data saved to {filename}")
            except Exception as e:
                error_msg = f"Save waveform error: {str(e)}"
                messagebox.showerror("Error", error_msg)
                logger.error(error_msg)


def main() -> None:
    """Main function to run the GUI"""
    # Load configuration
    config = Config()

    # Setup logging
    log_level = config.get('logging.level', 'INFO')
    log_file = config.get('logging.file')
    setup_logging(level=log_level, log_file=log_file)

    logger.info("Starting RIGOL Oscilloscope GUI")

    root = tk.Tk()

    # Configure styles
    style = ttk.Style()
    style.theme_use('clam')

    # Custom button styles
    style.configure('Green.TButton', foreground='green')
    style.configure('Red.TButton', foreground='red')

    app = OscilloscopeGUI(root, config)

    # Save config on exit
    def on_closing():
        config.save()
        root.destroy()

    root.protocol("WM_DELETE_WINDOW", on_closing)

    root.mainloop()

    logger.info("RIGOL Oscilloscope GUI closed")


if __name__ == "__main__":
    main()