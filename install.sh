#!/bin/bash

# ------------------------------------------------------------------------
# @brief Main installation script for PageStreamer
#
# This script:
# 1. Clones/updates the repository to ~/.pagestreamer
# 2. Installs all dependencies
# 3. Builds the C++ project
# 4. Configures the environment and stream settings
# 5. Adds the executable to PATH
# ------------------------------------------------------------------------

set -e # Exit on error

# Define installation directory
INSTALL_DIR="$HOME/.pagestreamer"
REPO_URL="https://github.com/cezou/PageStreamer.git"

echo "====== PageStreamer Installation ======"
echo "This will install PageStreamer to: $INSTALL_DIR"

# Check if Git is installed
if ! command -v git &>/dev/null; then
    echo "Git is not installed. Please install Git first."
    exit 1
fi

# Clone or update repository
if [ ! -d "$INSTALL_DIR" ]; then
    echo "Downloading PageStreamer..."
    git clone "$REPO_URL" "$INSTALL_DIR" &>/dev/null || {
        echo "Failed to clone repository. Please check your internet connection."
        exit 1
    }
else
    echo "Updating existing PageStreamer installation..."
    cd "$INSTALL_DIR"
    git pull &>/dev/null || echo "Warning: Could not update repository. Continuing with existing files."
fi

# Navigate to project directory
cd "$INSTALL_DIR"

# Create necessary directories
echo "Setting up directory structure..."
mkdir -p "$INSTALL_DIR/logs"
mkdir -p "$INSTALL_DIR/includes"
mkdir -p "$INSTALL_DIR/srcs"
mkdir -p "$INSTALL_DIR/obj"

# Install dependencies
echo "Installing dependencies..."
bash "$INSTALL_DIR/scripts/install_deps.sh"

# Build the C++ project
echo "Building PageStreamer..."
make

# Add to PATH
setup_path() {
    local shell_profile=""
    
    # Determine which shell profile to modify
    if [ -n "$ZSH_VERSION" ]; then
        shell_profile="$HOME/.zshrc"
    elif [ -n "$BASH_VERSION" ]; then
        if [[ "$OSTYPE" == "darwin"* ]]; then
            shell_profile="$HOME/.bash_profile"
        else
            shell_profile="$HOME/.bashrc"
        fi
    else
        shell_profile="$HOME/.profile"
    fi
    
    # Add to PATH if not already there
    if ! grep -q "export PATH=\"\$PATH:$INSTALL_DIR\"" "$shell_profile"; then
        echo "# Added by PageStreamer installer" >> "$shell_profile"
        echo "export PATH=\"\$PATH:$INSTALL_DIR\"" >> "$shell_profile"
        echo "PageStreamer added to PATH in $shell_profile"
        echo "Please restart your terminal or run: source $shell_profile"
    else
        echo "PageStreamer already in PATH"
    fi
    
    # Create symbolic link in ~/.local/bin if it exists
    if [ -d "$HOME/.local/bin" ]; then
        ln -sf "$INSTALL_DIR/PageStreamer" "$HOME/.local/bin/pagestreamer" 2>/dev/null
    fi
}

# Setup PATH
echo "Adding PageStreamer to PATH..."
setup_path

# Run configuration - Using printf instead of echo for escape sequences
printf "SQUID"
echo "Starting configuration..."
"$INSTALL_DIR/PageStreamer" --config

echo "===================================="
echo "Installation completed successfully!"
echo "PageStreamer is now available system-wide."
echo "You can use it with these commands:"
echo "  pagestreamer start  - Start the stream"
echo "  pagestreamer stop   - Stop the stream"
echo "  pagestreamer status - Check stream status"
echo "  pagestreamer --config - Change configuration"
echo "===================================="
