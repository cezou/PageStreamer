#!/bin/bash

# Répertoire du script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PARENT_DIR="$( cd "$DIR/.." && pwd )"
cd "$DIR"

# Vérifier si le fichier PID existe
if [ ! -f "$DIR/stream.pid" ]; then
    echo "Aucun stream en cours d'exécution (fichier PID non trouvé)."
    exit 1
fi

# Récupérer le PID et vérifier le processus
PID=$(cat "$DIR/stream.pid")
if ps -p $PID > /dev/null; then
    echo "Stream en cours d'exécution avec PID: $PID"
    
    # Afficher les 10 dernières lignes du log
    echo ""
    echo "Dernières lignes du log:"
    echo "------------------------"
    tail -n 10 "$PARENT_DIR/logs/stream.log"
    
    # Afficher quelques statistiques
    echo ""
    echo "Durée d'exécution:"
    ps -o etime= -p $PID
    
    echo ""
    echo "Utilisation des ressources:"
    ps -o %cpu,%mem,rss -p $PID
else
    echo "Le stream avec PID $PID n'est plus en cours d'exécution."
    echo "Le processus s'est peut-être arrêté de façon inattendue."
    echo "Consultez $PARENT_DIR/logs/stream.log pour plus d'informations."
    rm -f "$DIR/stream.pid"
    exit 1
fi

# Vérifier l'état des processus associés
echo ""
echo "Processus associés:"
echo "------------------"
pgrep -fa "ffmpeg|chromium|Xvfb" | grep -v "grep"

echo ""
echo "Pour arrêter le stream: bash stop_stream.sh"
