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
from utils import (setup_logging, format_measurement_value, validate_channel_number, 
                   validate_scale_value, validate_offset_value, validate_trigger_level,
                   validate_digital_channel, validate_digital_threshold, validate_digital_label,
                   is_valid_threshold_type)

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
        self.window_size = self.config.get('gui.window_size', '1920x1080')
        self.auto_update_rate = self.config.get('gui.auto_update_rate', 2.0)
        self.default_points = self.config.get('gui.default_points', 1000)
        
        # Track collapsible section states
        self.sections_collapsed = {}

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
        style.configure('Green.TButton', foreground='green', font=('Arial', 9, 'bold'))
        style.configure('Red.TButton', foreground='red', font=('Arial', 9, 'bold'))
        style.configure('Blue.TButton', foreground='blue', font=('Arial', 9, 'bold'))
        style.configure('Orange.TButton', foreground='orange', font=('Arial', 9, 'bold'))
        
        # Channel color styles
        style.configure('CH1.TLabel', foreground='#00ff00', font=('Arial', 9, 'bold'))
        style.configure('CH2.TLabel', foreground='#ffff00', font=('Arial', 9, 'bold'))
        style.configure('CH3.TLabel', foreground='#00ffff', font=('Arial', 9, 'bold'))
        style.configure('CH4.TLabel', foreground='#ff00ff', font=('Arial', 9, 'bold'))
        
        # Section header style
        style.configure('Header.TLabelframe.Label', font=('Arial', 10, 'bold'))
        style.configure('Header.TLabelframe', borderwidth=2, relief='raised')

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

        ttk.Button(toolbar, text="📷 Screenshot", command=self.take_screenshot).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="💾 Save Data", command=self.save_waveform).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="📊 FFT", command=self.toggle_fft_display, style="Blue.TButton").pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="📐 Math", command=self.show_math_functions).pack(side=tk.LEFT, padx=2)
        
        ttk.Separator(toolbar, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=5)
        
        ttk.Button(toolbar, text="🔍 Zoom", command=self.toggle_zoom_mode).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="📏 Cursors", command=self.toggle_cursors).pack(side=tk.LEFT, padx=2)
        
        # Statistics display
        self.acq_rate_label = ttk.Label(toolbar, text="Acq: 0 wfm/s", relief=tk.SUNKEN, width=12)
        self.acq_rate_label.pack(side=tk.RIGHT, padx=2)
        
        self.status_label = ttk.Label(toolbar, text="● Not Connected", foreground="red", font=('Arial', 9, 'bold'))
        self.status_label.pack(side=tk.RIGHT, padx=10)

        # Main container
        main_container = ttk.Frame(self.root)
        main_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Left panel - Analog Controls (with scrollbar)
        left_container = ttk.Frame(main_container, width=320)
        left_container.pack(side=tk.LEFT, fill=tk.Y, padx=(0, 3))
        left_container.pack_propagate(False)
        
        left_canvas = tk.Canvas(left_container, width=320, highlightthickness=0)
        left_scrollbar = ttk.Scrollbar(left_container, orient="vertical", command=left_canvas.yview)
        left_panel = ttk.Frame(left_canvas)
        
        left_panel.bind("<Configure>", lambda e: left_canvas.configure(scrollregion=left_canvas.bbox("all")))
        left_canvas.create_window((0, 0), window=left_panel, anchor="nw")
        left_canvas.configure(yscrollcommand=left_scrollbar.set)
        
        left_canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        left_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        # Middle panel - Waveform display and measurements
        middle_panel = ttk.Frame(main_container)
        middle_panel.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(0, 3))

        # Right panel - Digital Controls (with scrollbar)
        right_container = ttk.Frame(main_container, width=320)
        right_container.pack(side=tk.LEFT, fill=tk.Y)
        right_container.pack_propagate(False)
        
        right_canvas = tk.Canvas(right_container, width=320, highlightthickness=0)
        right_scrollbar = ttk.Scrollbar(right_container, orient="vertical", command=right_canvas.yview)
        right_panel = ttk.Frame(right_canvas)
        
        right_panel.bind("<Configure>", lambda e: right_canvas.configure(scrollregion=right_canvas.bbox("all")))
        right_canvas.create_window((0, 0), window=right_panel, anchor="nw")
        right_canvas.configure(yscrollcommand=right_scrollbar.set)
        
        right_canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        right_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        # Setup left panel sections - Analog controls
        self.setup_channel_controls(left_panel)
        self.setup_timebase_controls(left_panel)
        self.setup_trigger_controls(left_panel)
        self.setup_acquisition_controls(left_panel)

        # Setup middle panel - Waveform display and measurements
        self.setup_waveform_display(middle_panel)
        self.setup_measurements_panel(middle_panel)

        # Setup right panel - Digital controls
        self.setup_logic_analyzer_controls(right_panel)

    def setup_channel_controls(self, parent: ttk.Frame) -> None:
        """Setup channel control section"""
        # Collapsible header
        header_frame = ttk.Frame(parent)
        header_frame.pack(fill=tk.X, pady=2)
        
        collapse_btn = ttk.Button(header_frame, text="▼", width=3,
                                  command=lambda: self.toggle_section('channels'))
        collapse_btn.pack(side=tk.LEFT)
        ttk.Label(header_frame, text="Channel Controls", font=('Arial', 10, 'bold')).pack(side=tk.LEFT, padx=5)
        
        frame = ttk.LabelFrame(parent, text="", padding=5)
        frame.pack(fill=tk.X, pady=1)
        self.channel_controls_frame = frame
        self.sections_collapsed['channels'] = {'btn': collapse_btn, 'frame': frame, 'collapsed': False}

        self.channel_vars = {}
        self.channel_frames = {}

        for ch in range(1, 5):
            ch_frame = ttk.Frame(frame)
            ch_frame.pack(fill=tk.X, pady=2)

            # Channel enable with color indicator
            var = tk.BooleanVar(value=True if ch == 1 else False)
            self.channel_vars[ch] = var
            
            # Status LED indicator
            status_canvas = tk.Canvas(ch_frame, width=12, height=12, highlightthickness=0)
            status_canvas.pack(side=tk.LEFT, padx=2)
            colors = ['#00ff00', '#ffff00', '#00ffff', '#ff00ff']
            status_canvas.create_oval(2, 2, 10, 10, fill=colors[ch-1] if var.get() else 'gray', 
                                     outline=colors[ch-1], tags=f'ch{ch}_led')
            self.channel_vars[f'ch{ch}_led'] = status_canvas
            
            chk = ttk.Checkbutton(ch_frame, text=f"CH{ch}", variable=var,
                                  command=lambda c=ch: self.update_channel_display(c))
            chk.pack(side=tk.LEFT)

            # Scale
            ttk.Label(ch_frame, text="V/div:").pack(side=tk.LEFT, padx=(5, 2))
            scale_var = tk.StringVar(value=str(self.config.get('channels.default_scale', 1.0)))
            self.channel_vars[f'ch{ch}_scale'] = scale_var
            scale_combo = ttk.Combobox(ch_frame, textvariable=scale_var, width=6,
                                       values=['0.001', '0.002', '0.005', '0.01', '0.02', '0.05',
                                               '0.1', '0.2', '0.5', '1.0', '2.0', '5.0', '10.0'])
            scale_combo.pack(side=tk.LEFT, padx=2)
            scale_combo.bind('<<ComboboxSelected>>', lambda e, c=ch: self.update_channel_scale(c))

            # Offset
            ttk.Label(ch_frame, text="Ofs:").pack(side=tk.LEFT, padx=(5, 2))
            offset_var = tk.StringVar(value=str(self.config.get('channels.default_offset', 0.0)))
            self.channel_vars[f'ch{ch}_offset'] = offset_var
            offset_entry = ttk.Entry(ch_frame, textvariable=offset_var, width=5)
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
            ttk.Label(ch_frame2, text="Probe:").pack(side=tk.LEFT, padx=(5, 2))
            probe_var = tk.StringVar(value=str(self.config.get('channels.default_probe', 10.0)))
            self.channel_vars[f'ch{ch}_probe'] = probe_var
            probe_combo = ttk.Combobox(ch_frame2, textvariable=probe_var, width=5,
                                       values=['0.01', '0.1', '1', '10', '100', '1000'])
            probe_combo.pack(side=tk.LEFT, padx=2)
            probe_combo.bind('<<ComboboxSelected>>', lambda e, c=ch: self.update_channel_probe(c))

    def setup_logic_analyzer_controls(self, parent: ttk.Frame) -> None:
        """Setup logic analyzer control section"""
        frame = ttk.LabelFrame(parent, text="Logic Analyzer (Digital)", padding=5)
        frame.pack(fill=tk.BOTH, expand=True, pady=3)

        # Master enable/disable
        master_frame = ttk.Frame(frame)
        master_frame.pack(fill=tk.X, pady=3)
        
        self.la_enabled_var = tk.BooleanVar(value=False)
        ttk.Checkbutton(master_frame, text="Enable LA",
                        variable=self.la_enabled_var,
                        command=self.toggle_logic_analyzer).pack(side=tk.LEFT, padx=2)

        # Threshold settings
        threshold_frame = ttk.LabelFrame(frame, text="Threshold", padding=3)
        threshold_frame.pack(fill=tk.X, pady=3)

        ttk.Label(threshold_frame, text="Type:").pack(side=tk.LEFT, padx=2)
        self.la_threshold_var = tk.StringVar(value=self.config.get('logic_analyzer.default_threshold', 'TTL'))
        threshold_combo = ttk.Combobox(threshold_frame, textvariable=self.la_threshold_var, width=8,
                                       values=['TTL', 'CMOS5', 'CMOS3', 'ECL', 'LVTTL', 'LVCMOS3', 'LVCMOS2', 'CUSTOM'])
        threshold_combo.pack(side=tk.LEFT, padx=2)
        threshold_combo.bind('<<ComboboxSelected>>', lambda e: self.update_la_threshold())

        ttk.Label(threshold_frame, text="V:").pack(side=tk.LEFT, padx=(5, 2))
        self.la_threshold_level_var = tk.StringVar(value="1.5")
        threshold_entry = ttk.Entry(threshold_frame, textvariable=self.la_threshold_level_var, width=5)
        threshold_entry.pack(side=tk.LEFT, padx=2)
        threshold_entry.bind('<Return>', lambda e: self.update_la_threshold())

        # Digital channel selection with scrollable canvas
        channels_container = ttk.LabelFrame(frame, text="Channels (D0-D15)", padding=3)
        channels_container.pack(fill=tk.BOTH, expand=True, pady=3)

        # Create canvas with scrollbar - Expanded height for better visibility
        canvas = tk.Canvas(channels_container, height=400)
        scrollbar = ttk.Scrollbar(channels_container, orient="vertical", command=canvas.yview)
        scrollable_frame = ttk.Frame(canvas)

        scrollable_frame.bind(
            "<Configure>",
            lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
        )

        canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)

        canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        # Create controls for each digital channel
        self.digital_channel_vars = {}
        self.digital_label_vars = {}

        for d in range(16):
            d_frame = ttk.Frame(scrollable_frame)
            d_frame.pack(fill=tk.X, pady=2, padx=2)

            # Channel enable with status indicator
            var = tk.BooleanVar(value=False)
            self.digital_channel_vars[d] = var
            
            # Mini status LED
            led_canvas = tk.Canvas(d_frame, width=8, height=8, highlightthickness=0)
            led_canvas.pack(side=tk.LEFT, padx=1)
            digital_colors = ['#00ff00', '#ffff00', '#00ffff', '#ff00ff', '#ff8800', '#00ff88',
                             '#8800ff', '#ff0088', '#88ff00', '#0088ff', '#ff8888', '#88ff88',
                             '#8888ff', '#ffff88', '#ff88ff', '#88ffff']
            led_canvas.create_oval(1, 1, 7, 7, fill='gray', outline=digital_colors[d], tags=f'd{d}_led')
            self.digital_channel_vars[f'd{d}_led'] = led_canvas
            
            chk = ttk.Checkbutton(d_frame, text=f"D{d}", variable=var, width=4,
                                  command=lambda dc=d: self.update_digital_channel_display(dc))
            chk.pack(side=tk.LEFT)

            # Label
            ttk.Label(d_frame, text="Lbl:").pack(side=tk.LEFT, padx=(3, 2))
            label_var = tk.StringVar(value=f"D{d}")
            self.digital_label_vars[d] = label_var
            label_entry = ttk.Entry(d_frame, textvariable=label_var, width=5)
            label_entry.pack(side=tk.LEFT, padx=2)
            label_entry.bind('<Return>', lambda e, dc=d: self.update_digital_label(dc))
            label_entry.bind('<FocusOut>', lambda e, dc=d: self.update_digital_label(dc))

        # Bulk selection buttons
        bulk_frame = ttk.Frame(frame)
        bulk_frame.pack(fill=tk.X, pady=3)
        ttk.Button(bulk_frame, text="Enable All", command=self.enable_all_digital).pack(side=tk.LEFT, padx=2)
        ttk.Button(bulk_frame, text="Disable All", command=self.disable_all_digital).pack(side=tk.LEFT, padx=2)
        ttk.Button(bulk_frame, text="D0-D7", command=lambda: self.enable_digital_group(0, 7)).pack(side=tk.LEFT, padx=2)
        ttk.Button(bulk_frame, text="D8-D15", command=lambda: self.enable_digital_group(8, 15)).pack(side=tk.LEFT, padx=2)

    def setup_timebase_controls(self, parent: ttk.Frame) -> None:
        """Setup timebase control section"""
        frame = ttk.LabelFrame(parent, text="Timebase", padding=5)
        frame.pack(fill=tk.X, pady=3)

        # Time/div
        ttk.Label(frame, text="Time/div:").pack(side=tk.LEFT, padx=2)
        self.timebase_scale_var = tk.StringVar(value=str(self.config.get('timebase.default_scale', 1e-3)))
        timebase_combo = ttk.Combobox(frame, textvariable=self.timebase_scale_var, width=8,
                                      values=['1e-9', '2e-9', '5e-9', '1e-8', '2e-8', '5e-8',
                                              '1e-7', '2e-7', '5e-7', '1e-6', '2e-6', '5e-6',
                                              '1e-5', '2e-5', '5e-5', '1e-4', '2e-4', '5e-4',
                                              '1e-3', '2e-3', '5e-3', '1e-2', '2e-2', '5e-2',
                                              '1e-1', '2e-1', '5e-1', '1', '2', '5', '10'])
        timebase_combo.pack(side=tk.LEFT, padx=5)
        timebase_combo.bind('<<ComboboxSelected>>', lambda e: self.update_timebase())

        # Offset
        ttk.Label(frame, text="Offset:").pack(side=tk.LEFT, padx=(10, 2))
        self.timebase_offset_var = tk.StringVar(value=str(self.config.get('timebase.default_offset', 0.0)))
        offset_entry = ttk.Entry(frame, textvariable=self.timebase_offset_var, width=8)
        offset_entry.pack(side=tk.LEFT, padx=5)
        offset_entry.bind('<Return>', lambda e: self.update_timebase())

    def setup_trigger_controls(self, parent: ttk.Frame) -> None:
        """Setup trigger control section"""
        frame = ttk.LabelFrame(parent, text="Trigger", padding=5)
        frame.pack(fill=tk.X, pady=3)

        # Mode
        mode_frame = ttk.Frame(frame)
        mode_frame.pack(fill=tk.X, pady=2)
        ttk.Label(mode_frame, text="Mode:").pack(side=tk.LEFT, padx=2)
        self.trigger_mode_var = tk.StringVar(value=self.config.get('trigger.default_mode', 'AUTO'))
        for mode in ['AUTO', 'NORM', 'SING']:
            ttk.Radiobutton(mode_frame, text=mode, variable=self.trigger_mode_var, value=mode,
                            command=self.update_trigger).pack(side=tk.LEFT, padx=2)

        # Source
        source_frame = ttk.Frame(frame)
        source_frame.pack(fill=tk.X, pady=2)
        ttk.Label(source_frame, text="Source:").pack(side=tk.LEFT, padx=2)
        self.trigger_source_var = tk.StringVar(value=self.config.get('trigger.default_source', 'CHAN1'))
        source_combo = ttk.Combobox(source_frame, textvariable=self.trigger_source_var, width=8,
                                    values=['CHAN1', 'CHAN2', 'CHAN3', 'CHAN4', 'EXT', 'LINE'])
        source_combo.pack(side=tk.LEFT, padx=5)
        source_combo.bind('<<ComboboxSelected>>', lambda e: self.update_trigger())

        # Level
        level_frame = ttk.Frame(frame)
        level_frame.pack(fill=tk.X, pady=2)
        ttk.Label(level_frame, text="Level (V):").pack(side=tk.LEFT, padx=2)
        self.trigger_level_var = tk.StringVar(value=str(self.config.get('trigger.default_level', 0.0)))
        level_entry = ttk.Entry(level_frame, textvariable=self.trigger_level_var, width=8)
        level_entry.pack(side=tk.LEFT, padx=5)
        level_entry.bind('<Return>', lambda e: self.update_trigger())

        # Slope
        slope_frame = ttk.Frame(frame)
        slope_frame.pack(fill=tk.X, pady=2)
        ttk.Label(slope_frame, text="Slope:").pack(side=tk.LEFT, padx=2)
        self.trigger_slope_var = tk.StringVar(value=self.config.get('trigger.default_slope', 'POS'))
        for slope in ['POS', 'NEG', 'RFAL']:
            ttk.Radiobutton(slope_frame, text=slope, variable=self.trigger_slope_var, value=slope,
                            command=self.update_trigger).pack(side=tk.LEFT, padx=2)

        # Force trigger button
        ttk.Button(frame, text="Force Trigger", command=self.force_trigger).pack(pady=3)

    def setup_acquisition_controls(self, parent: ttk.Frame) -> None:
        """Setup acquisition control section"""
        frame = ttk.LabelFrame(parent, text="Acquisition", padding=5)
        frame.pack(fill=tk.X, pady=3)

        # Update rate
        rate_frame = ttk.Frame(frame)
        rate_frame.pack(fill=tk.X, pady=2)
        ttk.Label(rate_frame, text="Rate (Hz):").pack(side=tk.LEFT, padx=2)
        self.update_rate_var = tk.StringVar(value=str(self.auto_update_rate))
        rate_combo = ttk.Combobox(rate_frame, textvariable=self.update_rate_var, width=6,
                                  values=['0.5', '1', '2', '5', '10', '20'])
        rate_combo.pack(side=tk.LEFT, padx=5)

        # Number of points
        points_frame = ttk.Frame(frame)
        points_frame.pack(fill=tk.X, pady=2)
        ttk.Label(points_frame, text="Points:").pack(side=tk.LEFT, padx=2)
        self.points_var = tk.StringVar(value=str(self.default_points))
        points_combo = ttk.Combobox(points_frame, textvariable=self.points_var, width=6,
                                    values=['500', '1000', '2000', '5000', '10000'])
        points_combo.pack(side=tk.LEFT, padx=5)

        # Auto update checkbox
        self.auto_update_var = tk.BooleanVar(value=False)
        ttk.Checkbutton(frame, text="Auto Update",
                        variable=self.auto_update_var,
                        command=self.toggle_auto_update).pack(pady=3)

        # Manual update button
        ttk.Button(frame, text="Update Now",
                   command=self.update_waveform).pack(pady=3)

    def setup_waveform_display(self, parent: ttk.Frame) -> None:
        """Setup waveform display area"""
        frame = ttk.LabelFrame(parent, text="Waveform Display", padding=5)
        frame.pack(fill=tk.BOTH, expand=True, pady=(0, 5))

        # Create matplotlib figure with 2 subplots
        self.fig = Figure(figsize=(8, 6), dpi=100, facecolor='black')
        
        # Analog waveform subplot (top)
        self.ax = self.fig.add_subplot(211, facecolor='#001a00')
        self.ax.grid(True, color='#003300', linestyle='-', linewidth=0.5)
        self.ax.set_xlabel('Time', color='white', fontsize=9)
        self.ax.set_ylabel('Voltage (V)', color='white', fontsize=9)
        self.ax.tick_params(colors='white', labelsize=8)
        self.ax.spines['bottom'].set_color('white')
        self.ax.spines['top'].set_color('white')
        self.ax.spines['left'].set_color('white')
        self.ax.spines['right'].set_color('white')

        # Digital waveform subplot (bottom)
        self.ax_digital = self.fig.add_subplot(212, facecolor='#001a00')
        self.ax_digital.grid(True, color='#003300', linestyle='-', linewidth=0.5)
        self.ax_digital.set_xlabel('Time', color='white', fontsize=9)
        self.ax_digital.set_ylabel('Digital', color='white', fontsize=9)
        self.ax_digital.tick_params(colors='white', labelsize=8)
        self.ax_digital.spines['bottom'].set_color('white')
        self.ax_digital.spines['top'].set_color('white')
        self.ax_digital.spines['left'].set_color('white')
        self.ax_digital.spines['right'].set_color('white')
        self.ax_digital.set_ylim(-1, 16)
        self.ax_digital.set_yticks(range(16))
        self.ax_digital.set_yticklabels([f'D{i}' for i in range(16)])

        self.fig.tight_layout()

        # Create canvas
        self.canvas = FigureCanvasTkAgg(self.fig, master=frame)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        # Store line objects for each analog channel
        self.waveform_lines = {}
        colors = ['#00ff00', '#ffff00', '#00ffff', '#ff00ff']
        for i in range(1, 5):
            line, = self.ax.plot([], [], color=colors[i-1], linewidth=1.5, label=f'CH{i}')
            self.waveform_lines[i] = line

        self.ax.legend(loc='upper right', facecolor='black', edgecolor='white',
                       labelcolor='white', fontsize=8)

        # Store line objects for each digital channel
        self.digital_lines = {}
        digital_colors = ['#00ff00', '#ffff00', '#00ffff', '#ff00ff', '#ff8800', '#00ff88',
                         '#8800ff', '#ff0088', '#88ff00', '#0088ff', '#ff8888', '#88ff88',
                         '#8888ff', '#ffff88', '#ff88ff', '#88ffff']
        for i in range(16):
            line, = self.ax_digital.plot([], [], color=digital_colors[i], linewidth=1.0, 
                                         drawstyle='steps-post', label=f'D{i}')
            self.digital_lines[i] = line

    def setup_measurements_panel(self, parent: ttk.Frame) -> None:
        """Setup measurements display panel"""
        frame = ttk.LabelFrame(parent, text="Measurements", padding=5)
        frame.pack(fill=tk.X, pady=3)

        # Create measurement displays for each channel
        self.measurement_vars = {}

        for ch in range(1, 5):
            ch_frame = ttk.LabelFrame(frame, text=f"CH{ch}")
            ch_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=2)

            measurements = ['FREQ', 'VPP', 'VMAX', 'VMIN', 'VRMS', 'VAVG', 'PER', 'PWID']

            for meas in measurements:
                meas_frame = ttk.Frame(ch_frame)
                meas_frame.pack(fill=tk.X, pady=1)

                ttk.Label(meas_frame, text=f"{meas}:", width=5).pack(side=tk.LEFT)
                var = tk.StringVar(value="---")
                self.measurement_vars[f'ch{ch}_{meas}'] = var
                ttk.Label(meas_frame, textvariable=var, width=10,
                          relief=tk.SUNKEN).pack(side=tk.LEFT, fill=tk.X, expand=True)

        # Update measurements button
        ttk.Button(frame, text="Update Meas",
                   command=self.update_measurements).pack(pady=3)

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

    # Logic Analyzer control methods
    def toggle_logic_analyzer(self) -> None:
        """Toggle logic analyzer display"""
        if not self.scope:
            return
        try:
            state = self.la_enabled_var.get()
            self.scope.set_digital_display(state)
            
            # Update visibility of digital subplot
            if state:
                self.ax_digital.set_visible(True)
                self.fig.subplots_adjust(hspace=0.3)
            else:
                self.ax_digital.set_visible(False)
                # Hide all digital lines
                for line in self.digital_lines.values():
                    line.set_visible(False)
            
            self.canvas.draw()
            logger.debug(f"Logic analyzer display set to {state}")
        except Exception as e:
            error_msg = f"Logic analyzer toggle error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def update_la_threshold(self) -> None:
        """Update logic analyzer threshold"""
        if not self.scope:
            return
        try:
            threshold_type = self.la_threshold_var.get()
            
            if not is_valid_threshold_type(threshold_type):
                raise ValueError(f"Invalid threshold type: {threshold_type}")
            
            if threshold_type == 'CUSTOM':
                level_str = self.la_threshold_level_var.get()
                level = float(level_str)
                if not validate_digital_threshold(level):
                    raise ValueError(f"Invalid threshold level: {level}")
                self.scope.set_digital_threshold(threshold_type, level)
            else:
                self.scope.set_digital_threshold(threshold_type)
            
            logger.debug(f"Logic analyzer threshold updated to {threshold_type}")
        except Exception as e:
            error_msg = f"Threshold update error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def update_digital_channel_display(self, channel: int) -> None:
        """Update digital channel display state"""
        if not self.scope:
            return
        try:
            state = self.digital_channel_vars[channel].get()
            self.scope.set_digital_channel_display(channel, state)
            self.digital_lines[channel].set_visible(state)
            self.canvas.draw()
            logger.debug(f"Digital channel {channel} display updated to {state}")
        except Exception as e:
            error_msg = f"Digital channel display update error: {str(e)}"
            messagebox.showerror("Error", error_msg)
            logger.error(error_msg)

    def update_digital_label(self, channel: int) -> None:
        """Update digital channel label"""
        if not self.scope:
            return
        try:
            if not validate_digital_channel(channel):
                raise ValueError(f"Invalid digital channel: {channel}")
            
            label = self.digital_label_vars[channel].get()
            if not validate_digital_label(label):
                raise ValueError(f"Invalid label: '{label}' (max 4 characters)")
            
            self.scope.set_digital_label(channel, label)
            
            # Update the y-axis label
            labels = [self.digital_label_vars[i].get() for i in range(16)]
            self.ax_digital.set_yticklabels(labels)
            self.canvas.draw()
            
            logger.debug(f"Digital channel {channel} label updated to '{label}'")
        except Exception as e:
            error_msg = f"Digital channel label update error: {str(e)}"
            logger.error(error_msg)

    def enable_all_digital(self) -> None:
        """Enable all digital channels"""
        for d in range(16):
            self.digital_channel_vars[d].set(True)
            self.update_digital_channel_display(d)

    def disable_all_digital(self) -> None:
        """Disable all digital channels"""
        for d in range(16):
            self.digital_channel_vars[d].set(False)
            self.update_digital_channel_display(d)

    def enable_digital_group(self, start: int, end: int) -> None:
        """Enable a group of digital channels"""
        # First disable all
        self.disable_all_digital()
        # Then enable the requested group
        for d in range(start, end + 1):
            self.digital_channel_vars[d].set(True)
            self.update_digital_channel_display(d)

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

            # Get data for each enabled analog channel
            for ch in range(1, 5):
                if self.channel_vars[ch].get():
                    try:
                        time_data, voltage_data = self.scope.get_waveform_data(ch, points)
                        self.waveform_lines[ch].set_data(time_data, voltage_data)
                    except Exception as e:
                        logger.error(f"Error reading channel {ch}: {e}")

            # Get data for each enabled digital channel (if LA is enabled)
            if self.la_enabled_var.get():
                for d in range(16):
                    if self.digital_channel_vars[d].get():
                        try:
                            time_data, digital_data = self.scope.get_digital_data(d, points)
                            # Offset each digital channel vertically for display
                            digital_data_offset = digital_data * 0.8 + d
                            self.digital_lines[d].set_data(time_data, digital_data_offset)
                        except Exception as e:
                            logger.error(f"Error reading digital channel {d}: {e}")

            # Auto-scale axes
            self.ax.relim()
            self.ax.autoscale_view()
            
            if self.la_enabled_var.get():
                self.ax_digital.relim()
                self.ax_digital.autoscale_view(scaley=False)  # Keep Y fixed for digital

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
                    
                    # Add digital channels to header if LA is enabled
                    if self.la_enabled_var.get():
                        for d in range(16):
                            if self.digital_channel_vars[d].get():
                                label = self.digital_label_vars[d].get()
                                header += f",{label}"
                    
                    f.write(header + "\n")

                    # Get data for enabled analog channels
                    data = {}
                    time_data = None

                    for ch in range(1, 5):
                        if self.channel_vars[ch].get():
                            time_data, voltage_data = self.scope.get_waveform_data(ch, points)
                            data[f'CH{ch}'] = voltage_data

                    # Get data for enabled digital channels
                    if self.la_enabled_var.get():
                        for d in range(16):
                            if self.digital_channel_vars[d].get():
                                label = self.digital_label_vars[d].get()
                                if time_data is None:
                                    time_data, digital_data = self.scope.get_digital_data(d, points)
                                else:
                                    _, digital_data = self.scope.get_digital_data(d, points)
                                data[label] = digital_data

                    # Write data
                    if time_data is not None:
                        for i in range(len(time_data)):
                            line = f"{time_data[i]}"
                            
                            # Add analog channel data
                            for ch in range(1, 5):
                                if f'CH{ch}' in data:
                                    line += f",{data[f'CH{ch}'][i]}"
                            
                            # Add digital channel data
                            if self.la_enabled_var.get():
                                for d in range(16):
                                    if self.digital_channel_vars[d].get():
                                        label = self.digital_label_vars[d].get()
                                        if label in data:
                                            line += f",{int(data[label][i])}"
                            
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