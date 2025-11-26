#!/bin/bash

# Install system dependencies
sudo apt update
sudo apt install -y python3.12-tk

# Create virtual environment
python3 -m venv .venv

# Activate virtual environment first
source .venv/bin/activate

# Install requirements (note: corrected filename)
pip install -r requirements.txt