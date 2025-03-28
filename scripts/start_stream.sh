#!/bin/bash

# Répertoire du script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PARENT_DIR="$( cd "$DIR/.." && pwd )"
cd "$DIR"

# Nettoyer les processus existants avant de démarrer
bash ./stop_stream.sh > /dev/null 2>&1

# Créer dossier logs s'il n'existe pas
mkdir -p "$PARENT_DIR/logs"

# Rotation des logs avant de démarrer, peu importe l'heure
SKIP_STREAM_CHECK=1 bash ./rotate_logs.sh

# Démarrer Xvfb si nécessaire
if ! ps aux | grep -v grep | grep -q "Xvfb :99"; then
    # Supprimer le lock si existant
    if [ -e /tmp/.X99-lock ]; then
        echo "Display :99 déjà utilisé. Suppression du lock..."
        rm -f /tmp/.X99-lock
    fi

    echo "Démarrage de Xvfb..."
    Xvfb :99 -screen 0 1920x1080x24 &
    XVFB_PID=$!
    sleep 3  # Attendre que Xvfb démarre
    
    # Vérifier que Xvfb est lancé
    if ! ps -p $XVFB_PID > /dev/null; then
        echo "Erreur: Xvfb n'a pas démarré correctement."
        exit 1
    fi
else
    echo "Xvfb est déjà en cours d'exécution"
fi

# Démarrer PulseAudio si nécessaire
pulseaudio --check || pulseaudio --start

# Lancer le stream en arrière-plan avec les variables d'environnement définies
echo "Démarrage du stream en arrière-plan..."
(
    export DISPLAY=:99
    export XAUTHORITY=$HOME/.Xauthority
    export LAUNCHED_BY_START_SCRIPT=1
    
    # Mise en place d'un mécanisme de redémarrage automatique
    cd "$DIR"
    nohup node stream.js > "$PARENT_DIR/logs/stream.log" 2>&1 &
    
    PID=$!
    echo $PID > "$DIR/stream.pid"
    echo "Stream démarré avec PID: $PID"
)

# Vérifier si le processus a bien démarré
sleep 3
if ps -p $(cat "$DIR/stream.pid" 2>/dev/null) > /dev/null 2>&1; then
    echo "Stream démarré en arrière-plan avec PID: $(cat "$DIR/stream.pid")"
    echo "Consultez $PARENT_DIR/logs/stream.log pour voir les logs"
    echo "Pour arrêter le stream: bash stop_stream.sh"
else
    echo "AVERTISSEMENT: Le stream ne semble pas avoir démarré correctement."
    echo "Consultez $PARENT_DIR/logs/stream.log pour plus d'informations:"
    tail -n 10 "$PARENT_DIR/logs/stream.log"
fi

# Ajouter la tâche de rotation des logs à cron
(crontab -l 2>/dev/null | grep -v "rotate_logs.sh"; echo "*/30 * * * * cd $DIR && bash rotate_logs.sh >> $PARENT_DIR/logs/rotation.log 2>&1") | crontab -
