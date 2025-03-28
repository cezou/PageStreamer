#!/bin/bash

# Répertoire du script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PARENT_DIR="$( cd "$DIR/.." && pwd )"
cd "$DIR"

mkdir -p "$PARENT_DIR/logs"

LOG_FILE="$PARENT_DIR/logs/monitor.log"
TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S')

log() {
    echo "[$TIMESTAMP] $1" | tee -a "$LOG_FILE"
}

# Fonction pour vérifier si le stream est actif
check_stream() {
    # Vérifier si le processus stream.js est en cours d'exécution
    if [ -f "$DIR/stream.pid" ] && ps -p $(cat "$DIR/stream.pid") > /dev/null; then
        # Vérifier si ffmpeg est en cours d'exécution et envoie des données
        if pgrep -f "ffmpeg.*rtmp" > /dev/null; then
            # Vérifier l'activité récente de ffmpeg (moins de 2 minutes)
            FFMPEG_PID=$(pgrep -f "ffmpeg.*rtmp")
            LAST_ACTIVITY=$(ps -o etimes= -p $FFMPEG_PID)
            
            if [ "$LAST_ACTIVITY" -lt 7200 ]; then
                log "Stream en cours d'exécution normalement (activité ffmpeg: ${LAST_ACTIVITY}s)"
                return 0
            else
                log "Le processus ffmpeg semble figé (inactif depuis ${LAST_ACTIVITY}s)"
                return 1
            fi
        else
            log "Le processus Node.js est en cours d'exécution mais ffmpeg est arrêté"
            return 1
        fi
    else
        log "Le stream n'est pas en cours d'exécution"
        if [ -f "$DIR/stream.pid" ]; then
            log "PID file exists mais le processus est mort (PID: $(cat "$DIR/stream.pid"))"
        fi
        return 1
    fi
}

# Fonction pour redémarrer le stream si nécessaire
restart_if_needed() {
    if ! check_stream; then
        log "Tentative de redémarrage du stream..."
        
        # Vérifier si d'autres instances sont en cours d'exécution
        if pgrep -f "node stream.js" > /dev/null || pgrep -f "ffmpeg.*rtmp" > /dev/null; then
            log "Des processus stream résiduels existent, nettoyage forcé..."
            bash stop_stream.sh >> "$LOG_FILE" 2>&1
            sleep 5
        fi
        
        # Vérifier l'espace disque disponible
        DISK_SPACE=$(df -h /home | awk 'NR==2 {print $5}' | sed 's/%//')
        if [ "$DISK_SPACE" -gt 90 ]; then
            log "AVERTISSEMENT: Espace disque faible (${DISK_SPACE}%), nettoyage des anciens logs..."
            find "$PARENT_DIR/logs" -name "*.log.*" -type f -mtime +7 -delete
        fi
        
        log "Démarrage du stream..."
        bash start_stream.sh >> "$LOG_FILE" 2>&1
        
        # Vérifier si le redémarrage a réussi
        sleep 30
        if check_stream; then
            log "Stream redémarré avec succès"
        else
            log "ÉCHEC du redémarrage du stream!"
            
            # Vérifier les dernières erreurs
            if [ -f "$PARENT_DIR/logs/stream.log" ]; then
                log "Dernières erreurs:"
                tail -n 20 "$PARENT_DIR/logs/stream.log" | grep -i "error\|fail\|exception" >> "$LOG_FILE"
            fi
        fi
    fi
}

# Exécuter une vérification immédiate
restart_if_needed

# Afficher des statistiques du système
log "Statistiques système:"
echo "Utilisation CPU: $(top -bn1 | grep "Cpu(s)" | awk '{print $2}')%" >> "$LOG_FILE"
echo "Utilisation mémoire: $(free -m | awk 'NR==2{printf "%.2f%%", $3*100/$2}')" >> "$LOG_FILE"
echo "Uptime: $(uptime | cut -d',' -f1)" >> "$LOG_FILE"

log "Surveillance terminée"
