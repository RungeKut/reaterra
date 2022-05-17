#!/bin/sh
. ~/scripts/utils.sh
[ "$DEBUG_SCRIPT" = "1" ] && set -x || set +x

log "Update started..."

[ "$#" = "1" ] && RFILE="${1}" || RFILE=""

[ -f ~/version.txt ] && cp ~/version.txt ~/prev_version.txt


rm -rf ~/update_dir
mkdir ~/update_dir

KEY="$(cat ~/scripts/backup_key.txt)"
KEY="$(trim ${KEY})"

HDIR="$(echo ~)"

7z x -p${KEY} -t7z ${RFILE} -o${HDIR}/update_dir/

TARFILE="$(ls ${HDIR}/update_dir/)"

tar -xvf ${HDIR}/update_dir/${TARFILE} -C ${HDIR}/update_dir/

sed -i 's/\r//g' ~/update_dir/*.sh
sed -i 's/\r//g' ~/update_dir/scripts/*.sh
chmod +x ~/update_dir/scripts/*.sh
chmod +x ~/update_dir/*.sh

[ -f ~/update_dir/pre_update.sh ] && ~/update_dir/pre_update.sh

tar -xvf ${HDIR}/update_dir/${TARFILE} -C ${HDIR}/

sed -i 's/\r//g' ~/*.sh
sed -i 's/\r//g' ~/scripts/*.sh
chmod +x ~/scripts/*.sh
chmod +x ~/*.sh

[ -f ~/post_update.sh ] && ~/post_update.sh

rm -rf ~/update_dir

[ -f ~/pre_update.sh ] && rm -f ~/pre_update.sh
[ -f ~/post_update.sh ] && rm -f ~/post_update.sh

sudo reboot
