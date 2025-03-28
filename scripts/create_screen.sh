#!/bin/bash

# ------------------------------------------------------------------------
# @brief Sets up a virtual X11 display and audio for headless streaming
#
# This script initializes the virtual environment required for streaming:
# - Starts Xvfb to create a virtual display
# - Configures appropriate permissions
# - Sets up virtual audio devices
# - Sets environment variables
# ------------------------------------------------------------------------

# Kill any existing Xvfb process and remove lock if present
pkill Xvfb
if [ -e /tmp/.X99-lock ]; then
    echo "Display :99 already in use. Removing lock..."
    rm -f /tmp/.X99-lock
fi

# Create temporary directory for X files
XVFB_TMP="/tmp/xvfb_$USER"
mkdir -p $XVFB_TMP
chmod 700 $XVFB_TMP

# Launch Xvfb with additional options for stability
echo "Starting Xvfb..."
Xvfb :99 -screen 0 1920x1080x24 -ac -nolisten tcp -auth $XVFB_TMP/auth -dpi 96 &
XVFB_PID=$!

# Wait for Xvfb to fully initialize
echo "Waiting for Xvfb initialization..."
sleep 5

# Check if Xvfb is running
if ! ps -p $XVFB_PID > /dev/null; then
    echo "ERROR: Xvfb failed to start properly."
    exit 1
fi

# Ignore libEGL and xkbcomp warnings
export LIBGL_ALWAYS_SOFTWARE=1
export LIBGL_DEBUG=quiet

# Export DISPLAY for child processes
export DISPLAY=:99
echo "DISPLAY set to :99"

# Configure audio permissions
echo "Configuring PulseAudio..."
# Start PulseAudio if needed
if ! pulseaudio --check; then
    pulseaudio --start
    sleep 2
fi

# Create virtual audio device with enhanced parameters
if ! pactl list | grep -q "virt_output"; then
    echo "Creating virtual audio device..."
    pactl load-module module-null-sink sink_name=virt_output sink_properties=device.description="Virtual_Output" rate=48000 channels=2
fi

# Ensure correct permissions
echo "Configuring permissions..."
if command -v xhost &>/dev/null; then
    xhost +local: || true
fi

echo "Environment setup completed successfully"
