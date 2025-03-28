#!/bin/bash

# Répertoire du script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PARENT_DIR="$( cd "$DIR/.." && pwd )"
cd "$DIR"

# Extraire la clé de stream depuis le fichier .env dans le répertoire parent
if [ -f "$PARENT_DIR/.env" ]; then
    STREAM_KEY=$(grep -oP "STREAM_KEY=\K[^']+" "$PARENT_DIR/.env" || echo "")
    if [ -z "$STREAM_KEY" ]; then
        echo "AVERTISSEMENT: Clé de streaming non trouvée dans $PARENT_DIR/.env"
    else
        echo "Clé de streaming trouvée"
    fi
else
    echo "ERREUR: Fichier .env non trouvé dans $PARENT_DIR"
    STREAM_KEY=""
fi

# Arrêter les processus dans l'ordre correct
echo "Arrêt des processus stream..."

# 1. Arrêter node.js en premier
if [ -f stream.pid ]; then
    PID=$(cat stream.pid)
    if ps -p $PID > /dev/null 2>&1; then
        echo "Arrêt du processus Node.js (PID: $PID)..."
        kill $PID
        sleep 2
        if ps -p $PID > /dev/null 2>&1; then
            echo "Force kill du processus Node.js..."
            kill -9 $PID
        fi
    else
        echo "Le processus Node.js avec PID $PID n'est plus en cours d'exécution."
    fi
    rm -f stream.pid
else
    echo "Recherche de processus Node.js en cours..."
    NODE_PIDS=$(pgrep -f "node stream.js")
    if [ ! -z "$NODE_PIDS" ]; then
        echo "Processus Node.js trouvés: $NODE_PIDS"
        kill $NODE_PIDS 2>/dev/null
    fi
fi

# 2. Arrêter ffmpeg
echo "Arrêt des processus ffmpeg..."
pkill -f "ffmpeg.*pulse" 2>/dev/null
pkill -f "ffmpeg.*x11grab" 2>/dev/null
if [ ! -z "$STREAM_KEY" ]; then
    pkill -f "ffmpeg.*$STREAM_KEY" 2>/dev/null
fi

# 3. Arrêter chromium
echo "Arrêt des processus Chromium..."
pkill -f chromium 2>/dev/null

# 4. Arrêter Xvfb en dernier
echo "Arrêt de Xvfb..."
pkill Xvfb 2>/dev/null

# Nettoyer les fichiers de verrou
if [ -e /tmp/.X99-lock ]; then
    echo "Suppression du lock X11..."
    rm -f /tmp/.X99-lock
fi

echo "Tous les processus ont été arrêtés."
echo "Vous pouvez redémarrer le stream avec 'bash start_stream.sh'"
