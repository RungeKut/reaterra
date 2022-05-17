#!/bin/sh
. ~/scripts/utils.sh
[ "$DEBUG_SCRIPT" = "1" ] && set -x || set +x

#[ "$(systemctl is-active --quiet hostapd && echo 1 || echo 0)" = "0" ] && { log "WiFi is not set as Access Point. Execution is cancelled"; return 1; } 

if [ "$#" != "1" ] || [ "$(get4N "$1")" = "" ]; then
    echo "Should be one numeric argument: set_id NNNN"
    log "Arguments error!"
    return 0
fi

echo "id $1" > id.txt

NEWNNN="$(get4N "$1")"
SSID=""
while IFS='=' read -r key value; do
    [ "$key" = "ssid" ] && SSID=$(trim ${value})
done < /etc/hostapd/hostapd.conf

[ "$SSID" = "" ] && { log "SSID is empty. Execution cancelled."; return 1; }

STRLEN=${#SSID}                                              

[ "$STRLEN" -lt "5" ] && { log "SSID is to short. Execution cancelled."; return 1; }

PREV="$(echo ${SSID} | rev | cut -c -4 | rev)"
NNN="$(get4N "$PREV")"
NAME="$(echo ${SSID} | rev | cut -c 5- | rev)"

[ "$NNN" = "" ] && { log "SSID should be ended by 4 digits. Execution cancelled."; return 1; }

log "WiFi Access Point network ID change..."

NEWNAME="${NAME}${NEWNNN}"

sudo sed -i "s/^\(ssid=\).*/\1${NEWNAME}/" /etc/hostapd/hostapd.conf
[ -f /etc/wifi/RT2870AP/RT2870AP.txt ] && sudo sed -i "s/^\(SSID=\).*/\1${NEWNAME}/" /etc/wifi/RT2870AP/RT2870AP.txt

sudo reboot

#sudo systemctl enable dnsmasq.service
#sudo systemctl enable hostapd.service
#sudo systemctl stop dnsmasq.service
#sudo systemctl stop hostapd.service
#sudo systemctl start dnsmasq.service
#sudo systemctl start hostapd.service

log " -> COMPLETED"
