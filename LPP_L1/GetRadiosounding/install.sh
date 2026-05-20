#!/bin/bash

# Create a virtual environment named "venv" if it doesn't already exist
if [ ! -d "venv" ]; then
    echo "Creating virtual environment 'venv'..."
    python3 -m venv venv
else
    echo "Virtual environment 'venv' already exists."
fi

# Activate the virtual environment
echo "Activating virtual environment..."
source venv/bin/activate

# Install the required dependencies
echo "Installing dependencies from requirements.txt..."
pip install --upgrade pip
pip install -r requirements.txt

echo "Installation complete."
echo ""
echo "To use the scripts, activate the virtual environment with:"
echo "source venv/bin/activate"
echo "and run: python get_UWyoming_snd.py [arguments]"
echo ""
echo "Alternatively, use the execute.sh wrapper without manually activating the environment:"
echo "./execute.sh [arguments]"
echo ""
echo "Example execution:"
echo "./execute.sh -y 2021 -m 9 -i 0100 -e 0200 -r samer -s 87576"

# Ensure execute.sh is executable
chmod +x execute.sh
