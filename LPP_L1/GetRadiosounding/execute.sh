#!/bin/bash

# Create a virtual environment named "venv" if it doesn't already exist
if [ ! -d "venv" ]; then
    echo "Virtual environment 'venv' not found. Please run ./install.sh first."
    exit 1
fi

# Activate the virtual environment
source venv/bin/activate

# Execute the python script with all passed arguments
python get_UWyoming_snd.py "$@"

# Deactivate the virtual environment
deactivate
