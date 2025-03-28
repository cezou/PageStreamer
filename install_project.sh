#!/bin/bash

# Définir l'URL du dépôt GitHub
REPO_URL="https://github.com/cezou/LetMeEatMoreThan42Mins.git"

# Définir le répertoire d'installation
INSTALL_DIR="$HOME/.LetMeEatMoreThan42Mins"

# Vérifier si Git est installé
if ! command -v git &>/dev/null; then
    echo "Git is not installed. Please install Git first."
    exit 1
fi

# Cloner le dépôt Git dans le répertoire d'installation
if [ ! -d "$INSTALL_DIR" ]; then
    echo "Downloading LetMeEatMoreThan42Mins..."
    git clone "$REPO_URL" "$INSTALL_DIR" &>/dev/null
    if [ $? -ne 0 ]; then
        echo "Failed to clone repository. Please check your internet connection and try again."
        exit 1
    fi
else
    echo "Updating LetMeEatMoreThan42Mins..."
    cd "$INSTALL_DIR"
    git pull &>/dev/null
    if [ $? -ne 0 ]; then
        echo "Failed to update repository. Continuing with existing files."
    fi
fi

# Se rendre dans le répertoire du projet
cd "$INSTALL_DIR"

# Exécuter le script d'installation (install.sh)
echo "Running installation script..."
bash "$INSTALL_DIR/install.sh"

echo "Installation complete!"
echo "You can now use PageStreamer with: pagestreamer [start|stop|status]"
