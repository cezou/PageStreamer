#!/bin/bash

# ------------------------------------------------------------------------
# @brief Installs all dependencies required for the streaming system
#
# This script installs:
# - Node.js and NPM
# - Puppeteer dependencies
# - Xvfb and PulseAudio
# - FFmpeg
# - Development tools for C++ compilation
# ------------------------------------------------------------------------

set -e # Exit on error

echo "Installing system dependencies..."

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$NAME
    VER=$VERSION_ID
elif [ -f /etc/debian_version ]; then
    OS="Debian"
    VER=$(cat /etc/debian_version)
else
    OS=$(uname -s)
    VER=$(uname -r)
fi

echo "Detected OS: $OS $VER"

# Function to install dependencies based on the OS
install_dependencies() {
    echo "Updating package lists..."
    
    if [[ "$OS" == *"Ubuntu"* ]] || [[ "$OS" == *"Debian"* ]]; then
        # For Ubuntu/Debian
        sudo apt-get update -qq

        echo "Installing Xvfb, pulseaudio, and browser dependencies..."
        sudo apt-get install -y -qq \
            xvfb \
            pulseaudio \
            chromium-browser \
            ffmpeg \
            build-essential \
            g++ \
            curl \
            git \
            xvfb
    elif [[ "$OS" == *"CentOS"* ]] || [[ "$OS" == *"Red Hat"* ]] || [[ "$OS" == *"Fedora"* ]]; then
        # For CentOS/RHEL/Fedora
        sudo yum update -y -q
        
        sudo yum install -y -q \
            xorg-x11-server-Xvfb \
            pulseaudio \
            chromium \
            ffmpeg \
            gcc-c++ \
            make \
            curl \
            git
    else
        echo "Unsupported OS: $OS"
        echo "Please install the required dependencies manually:"
        echo "- Node.js and npm"
        echo "- Xvfb"
        echo "- PulseAudio"
        echo "- FFmpeg"
        echo "- Chromium or Chrome"
        echo "- g++ and make"
        exit 1
    fi

    echo "System dependencies installed successfully."
}

# Function to install or update Node.js
install_nodejs() {
    echo "Checking Node.js installation..."
    
    # Check if Node.js is installed and its version
    if command -v node &>/dev/null; then
        NODE_VERSION=$(node -v | cut -d "v" -f 2 | cut -d "." -f 1)
        if [ "$NODE_VERSION" -ge 18 ]; then
            echo "Node.js version $(node -v) is already installed and compatible."
            return
        else
            echo "Node.js $(node -v) is installed but too old (v18+ required)."
        fi
    else
        echo "Node.js not found."
    fi
    
    echo "Installing/updating Node.js..."
    
    # Install Node.js using NVM
    if ! command -v nvm &>/dev/null; then
        echo "Installing NVM..."
        curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.3/install.sh | bash
        
        # Load NVM
        export NVM_DIR="$HOME/.nvm"
        [ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"
    fi
    
    # Install latest LTS version instead of a specific version
    nvm install --lts
    nvm use --lts
    
    # Check if npm is installed
    if ! command -v npm &>/dev/null; then
        echo "npm not found. Please install npm manually."
        exit 1
    else
        NPM_VERSION=$(npm -v)
        echo "npm version $NPM_VERSION is installed."
    fi
}

# Function to install Node project dependencies
install_node_dependencies() {
    echo "Installing Node.js project dependencies..."
    cd "$PROJECT_ROOT"
    
    npm install
    
    echo "Node.js dependencies installed successfully."
}

# Main installation process
PROJECT_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

# Install system dependencies
install_dependencies

# Install or update Node.js
install_nodejs

# Install Node.js project dependencies
install_node_dependencies

echo "All dependencies installed successfully."
