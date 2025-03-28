#!/bin/bash

# Répertoire du script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PARENT_DIR="$( cd "$DIR/.." && pwd )"

# Configuration
LOG_DIR="$PARENT_DIR/logs"
MAX_SIZE_MB=20    # Taille maximale en MB avant rotation
MAX_FILES=15      # Nombre maximal de fichiers de logs à conserver
MAX_AGE_DAYS=14   # Nombre de jours maximum pour conserver les logs

# Vérifier si nous sommes en période de stream
if [ "$SKIP_STREAM_CHECK" != "1" ]; then
    if ! bash "$DIR/check_stream_time.sh" > /dev/null; then
        echo "Hors période de stream ou processus non actif. Rotation ignorée."
        exit 0
    fi
    echo "En période de stream, rotation des logs activée."
fi

# Vérifier si le répertoire des logs existe
if [ ! -d "$LOG_DIR" ]; then
    mkdir -p "$LOG_DIR"
    echo "Répertoire de logs créé: $LOG_DIR"
fi

# Fonction pour vérifier la taille d'un fichier en MB
get_file_size() {
    local file="$1"
    if [ -f "$file" ]; then
        du -m "$file" | cut -f1
    else
        echo "0"
    fi
}

# Fonction pour faire la rotation des logs
rotate_log() {
    local log_file="$1"
    local timestamp=$(date +"%Y%m%d_%H%M%S")
    
    # Déplacer le fichier actuel avec un timestamp
    if [ -f "$log_file" ] && [ -s "$log_file" ]; then
        # Compresser les anciens logs pour économiser de l'espace
        gzip -c "$log_file" > "${log_file}.${timestamp}.gz"
        
        # Vider le fichier de log actuel
        : > "$log_file"
        
        echo "Log rotaté et compressé: ${log_file} -> ${log_file}.${timestamp}.gz"
    else
        echo "Pas de rotation nécessaire pour $log_file (vide ou inexistant)"
    fi
    
    # Nettoyer les anciens fichiers si nécessaire
    cleanup_old_logs "$log_file"
}

# Fonction pour nettoyer les anciens fichiers
cleanup_old_logs() {
    local base_name=$(basename "$1")
    
    # Supprimer les fichiers plus anciens que MAX_AGE_DAYS jours
    find "$LOG_DIR" -name "${base_name}.*" -type f -mtime +$MAX_AGE_DAYS -delete
    
    # Limiter le nombre de fichiers
    local count=$(find "$LOG_DIR" -name "${base_name}.*" | wc -l)
    if [ "$count" -gt "$MAX_FILES" ]; then
        local excess=$((count - MAX_FILES))
        echo "Suppression de $excess anciens fichiers de logs..."
        find "$LOG_DIR" -name "${base_name}.*" -type f -printf "%T@ %p\n" | \
            sort -n | head -n "$excess" | cut -d' ' -f2- | xargs rm -f
    fi
}

# Vérifier l'espace disque disponible
DISK_SPACE=$(df -h $LOG_DIR | awk 'NR==2 {print $5}' | sed 's/%//')
if [ "$DISK_SPACE" -gt 85 ]; then
    echo "AVERTISSEMENT: Espace disque faible (${DISK_SPACE}%), nettoyage agressif..."
    find "$LOG_DIR" -name "*.log.*" -type f -mtime +1 -delete
    find "$LOG_DIR" -name "*.log" -type f -size +${MAX_SIZE_MB}M -exec : > {} \;
fi

# Traiter tous les fichiers de log dans le répertoire
for log_file in "$LOG_DIR"/*.log; do
    if [ -f "$log_file" ]; then
        file_name=$(basename "$log_file")
        size=$(get_file_size "$log_file")
        if [ "$size" -ge "$MAX_SIZE_MB" ]; then
            echo "Le fichier $file_name dépasse ${MAX_SIZE_MB}MB (taille actuelle: ${size}MB)"
            rotate_log "$log_file"
        else
            echo "Taille de $file_name: ${size}MB (max: ${MAX_SIZE_MB}MB)"
        fi
    fi
done

echo "Rotation des logs terminée à $(date)"
