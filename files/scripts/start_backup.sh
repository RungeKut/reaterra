#!/bin/sh
. ~/scripts/utils.sh
[ "$DEBUG_SCRIPT" = "1" ] && set -x || set +x

log "Backup started..."

[ "$#" = "1" ] && NAME="${1}_" || NAME=""

rm -rf ~/update_dir
rm -rf ~/backup_dir
mkdir ~/backup_dir

[ -f ~/ext_backup.sh ] && ~/ext_backup.sh

KEY="$(cat ~/scripts/backup_key.txt)"
KEY="$(trim ${KEY})"
FILENAME="backup_${NAME}$(date +%Y%m%d_%H%M%S)"

tar -cpv --exclude='~/backup_dir' -f ~/backup_dir/${FILENAME}.tar .
7z a -p${KEY} -mx=9 -mhe -t7z -sdel ~/backup_dir/${FILENAME}.zip ~/backup_dir/${FILENAME}.tar

log " -> COMPLETED"
