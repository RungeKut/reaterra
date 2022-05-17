#!/bin/sh
. ~/scripts/utils.sh
[ "$DEBUG_SCRIPT" = "1" ] && set -x || set +x

log "ext_backup started..." 

while !(mysqladmin ping)
do
   sleep 1
   log "Waiting for mariadb ..."
done
mysqldump REATERRA -ureauser -prEa2AeR > db_backup.sql