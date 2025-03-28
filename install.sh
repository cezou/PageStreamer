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

# Create bin directory and symlink
mkdir -p "$INSTALL_DIR/bin"
ln -sf "$INSTALL_DIR/pagestreamer" "$INSTALL_DIR/bin/pagestreamer"

# Add to PATH - improved version for all environments
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
    
    # Create directories for links
    mkdir -p "$HOME/.local/bin"
    
    # Add multiple options for PATH to ensure it works
    
    # 1. Add executable directory to PATH
    if ! grep -q "export PATH=\"\$PATH:$INSTALL_DIR\"" "$shell_profile"; then
        echo "" >> "$shell_profile"
        echo "# Added by PageStreamer installer" >> "$shell_profile"
        echo "export PATH=\"\$PATH:$INSTALL_DIR\"" >> "$shell_profile"
        echo "Path added to $shell_profile"
    fi
    
    # 2. Add bin subdirectory to PATH
    if ! grep -q "export PATH=\"\$PATH:$INSTALL_DIR/bin\"" "$shell_profile"; then
        echo "export PATH=\"\$PATH:$INSTALL_DIR/bin\"" >> "$shell_profile"
        echo "Bin path added to $shell_profile"
    fi
    
    # 3. Create symlink in ~/.local/bin which should be in PATH
    ln -sf "$INSTALL_DIR/pagestreamer" "$HOME/.local/bin/pagestreamer"
    echo "Symlink created in ~/.local/bin"
    
    # 4. Add ~/.local/bin to PATH if not already there
    if ! grep -q "export PATH=\"\$PATH:\$HOME/.local/bin\"" "$shell_profile"; then
        echo "export PATH=\"\$PATH:\$HOME/.local/bin\"" >> "$shell_profile"
        echo "~/.local/bin added to PATH in $shell_profile"
    fi
    
    # 5. Create direct alias as a fallback
    if ! grep -q "alias pagestreamer=\"$INSTALL_DIR/pagestreamer\"" "$shell_profile"; then
        echo "alias pagestreamer=\"$INSTALL_DIR/pagestreamer\"" >> "$shell_profile"
        echo "Alias created in $shell_profile"
    fi
    
    # Apply PATH changes to current session
    export PATH="$PATH:$INSTALL_DIR:$INSTALL_DIR/bin:$HOME/.local/bin"
    alias pagestreamer="$INSTALL_DIR/pagestreamer"
    
    echo ""
    echo "PATH has been updated with multiple options to ensure the command works."
    echo "The pagestreamer command should now be available after you run:"
    echo "  source $shell_profile"
}

# Setup PATH
echo "Adding PageStreamer to PATH..."
setup_path

# Provide direct executable path for immediate use
echo ""
echo "You can use PageStreamer immediately by running:"
echo "  $INSTALL_DIR/pagestreamer"
echo ""

# Run configuration - Using clear command instead of escape sequences
clear
echo "Starting configuration..."
"$INSTALL_DIR/pagestreamer" --config

echo "===================================="
echo "Installation completed successfully!"
echo ""
echo "PageStreamer is now available via:"
echo "  $INSTALL_DIR/pagestreamer (direct path)"
echo ""
echo "To make the 'pagestreamer' command available, run:"
echo "  source $HOME/.$(basename $SHELL)rc"
echo ""
echo "After sourcing your shell profile, you can use:"
echo "  pagestreamer start  - Start the stream"
echo "  pagestreamer stop   - Stop the stream"
echo "  pagestreamer status - Check stream status"
echo "  pagestreamer --config - Change configuration"
echo "===================================="

# Temporary solution for immediate use
chmod +x "$INSTALL_DIR/pagestreamer"
echo ""
echo "For immediate use without restarting your terminal:"
export PATH="$PATH:$INSTALL_DIR"
echo "$ export PATH=\"\$PATH:$INSTALL_DIR\""
echo "$ pagestreamer status"
