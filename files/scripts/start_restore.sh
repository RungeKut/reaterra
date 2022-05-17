#!/bin/sh
. ~/scripts/utils.sh
[ "$DEBUG_SCRIPT" = "1" ] && set -x || set +x

log "Restore started..."

[ "$#" = "1" ] && RFILE="${1}" || RFILE=""

rm -rf ~/restore_dir
mkdir ~/restore_dir

KEY="$(cat ~/scripts/backup_key.txt)"
KEY="$(trim ${KEY})"

HDIR="$(echo ~)"

7z x -p${KEY} -t7z ${RFILE} -o${HDIR}/restore_dir/

TARFILE="$(ls ${HDIR}/restore_dir/)"

tar -xvf ${HDIR}/restore_dir/${TARFILE} -C ${HDIR}/

[ -f ~/ext_restore.sh ] && ~/ext_restore.sh

rm -rf ~/restore_dir
