#!/bin/bash

# ------------------------------------------------------------------------
# @brief Main installation script for RouletteTV Streaming System
#
# This script:
# 1. Initializes the project structure
# 2. Runs the dependency installation script
# 3. Builds the C++ project
# 4. Configures the environment
# ------------------------------------------------------------------------

set -e # Exit on error

# Get the project root directory
PROJECT_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$PROJECT_ROOT"

echo "==== RouletteTV Streaming System Installation ===="
echo "Project location: $PROJECT_ROOT"

# Create necessary directories if they don't exist
echo "Setting up directory structure..."
mkdir -p "$PROJECT_ROOT/logs"
mkdir -p "$PROJECT_ROOT/includes"
mkdir -p "$PROJECT_ROOT/srcs"
mkdir -p "$PROJECT_ROOT/obj"

# Install dependencies
echo "Installing dependencies..."
bash "$PROJECT_ROOT/scripts/install_deps.sh"

# Check if environment file exists
if [ ! -f "$PROJECT_ROOT/.env" ]; then
    echo "Creating .env file..."
    cat > "$PROJECT_ROOT/.env" << EOL
# Clé de streaming YouTube
STREAM_KEY=your_stream_key_here
# Plateforme de streaming
PLATFORM=rtmp://a.rtmp.youtube.com/live2
EOL
    echo "Please edit the .env file to add your streaming key"
fi

# Install cron jobs if needed
if [ ! -f "$PROJECT_ROOT/crontab.txt" ]; then
    echo "Setting up default crontab configuration..."
    cp "$PROJECT_ROOT/scripts/crontab.default.txt" "$PROJECT_ROOT/crontab.txt"
    echo "Review the crontab.txt file to adjust the streaming schedule"
fi

# Build the C++ project
echo "Building PageStreamer utility..."
cd "$PROJECT_ROOT" && make

# Create executable links
echo "Setting up executable links..."
ln -sf "$PROJECT_ROOT/PageStreamer" "$HOME/.local/bin/pagestreamer" 2>/dev/null || \
    echo "Could not create executable link. You can run the program with $PROJECT_ROOT/PageStreamer"

echo "=============================="
echo "Installation completed successfully!"
echo "You can now use the system with these commands:"
echo "  pagestreamer start  - Start the stream"
echo "  pagestreamer stop   - Stop the stream"
echo "  pagestreamer status - Check stream status"
echo ""
echo "Don't forget to edit your .env file with your streaming key!"
echo "=============================="
