.PHONY: all install run test clean setup build venv help distclean uninstall

# Default target
all: venv install

# Create virtual environment
venv:
	python3 -m venv .venv

# Install dependencies in virtual environment
install: venv
	.venv/bin/pip install -r requirements.txt

# Run the application
run: venv
	.venv/bin/python src/rigol_gui.py

# Run tests
test: venv
	.venv/bin/python src/test_components.py

# Clean build artifacts and cache
clean:
	find . -type d -name __pycache__ -exec rm -rf {} +
	find . -type f -name "*.pyc" -delete
	find . -type d -name "*.egg-info" -exec rm -rf {} +
	rm -rf build/ dist/ *.spec

# Deep clean including virtual environment
distclean: clean
	rm -rf .venv/

# Uninstall packages
uninstall: venv
	.venv/bin/pip uninstall -r requirements.txt -y

# Setup environment (legacy, use 'make all' instead)
setup: all

# Build standalone executable
build: venv install
	.venv/bin/pyinstaller --onefile --windowed --name rigol_gui src/rigol_gui.py

# Show help
help:
	@echo "Available targets:"
	@echo "  all        - Create venv and install dependencies (default)"
	@echo "  venv       - Create virtual environment"
	@echo "  install    - Install Python dependencies"
	@echo "  run        - Run the GUI application"
	@echo "  test       - Run tests"
	@echo "  clean      - Clean cache and build artifacts"
	@echo "  distclean  - Deep clean including venv"
	@echo "  uninstall  - Uninstall Python packages"
	@echo "  setup      - Legacy setup (same as all)"
	@echo "  build      - Build standalone executable"
	@echo "  help       - Show this help message"