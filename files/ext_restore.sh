#!/bin/sh
. ~/scripts/utils.sh
[ "$DEBUG_SCRIPT" = "1" ] && set -x || set +x

log "ext_backup started..." 


if [ -f db_backup.sql ]; then
    log "DB backup file is found. Try to restore from it..."
    mariadb -u reauser -prEa2AeR REATERRA < db_backup.sql
fi