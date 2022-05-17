#!/bin/sh
. ~/scripts/utils.sh
[ "$DEBUG_SCRIPT" = "1" ] && set -x || set +x

if ( [ "$#" != "3" ] && [ "$#" != "1" ] ) || ( [ "$1" != "ap" ] && [ "$1" != "cl" ] ); then
    echo "Should be three arguments: set_wifi ap|cl [ssid key]"
    log "Arguments error!"
    return 0
fi

if [ "$1" = "ap" ]; then
    log "WiFi Access Point setup..."
    if  [ "$#" = "3" ]; then 	
        sudo sed -i "s/^\(ssid=\).*/\1$2/" /etc/hostapd/hostapd.conf
        sudo sed -i "s/^\(wpa_passphrase=\).*/\1$3/" /etc/hostapd/hostapd.conf
    fi
    sudo sed -i "s/^#-ap //" /etc/dhcpcd.conf
    sudo systemctl restart dhcpcd
    sudo systemctl enable dnsmasq.service
    sudo systemctl enable hostapd.service
    sudo systemctl stop dnsmasq.service
    sudo systemctl stop hostapd.service
    sudo systemctl start dnsmasq.service
    sudo systemctl start hostapd.service
else
    log "WiFi Client setup..."	
    if  [ "$#" = "3" ]; then 	
        sudo sed -i "s/\(^.*ssid=\).*/\1\"$2\"/" /etc/wpa_supplicant/wpa_supplicant.conf
        sudo sed -i "s/\(^.*psk=\).*/\1\"$3\"/" /etc/wpa_supplicant/wpa_supplicant.conf
    fi
    sudo systemctl stop dnsmasq.service
    sudo systemctl stop hostapd.service
    sudo systemctl disable dnsmasq.service
    sudo systemctl disable hostapd.service
    sudo sed -i "s/^\s*interface /#-ap &/" /etc/dhcpcd.conf
    sudo sed -i "s/^\s*static ip_address=192.168.0.1\/24/#-ap &/" /etc/dhcpcd.conf
    sudo sed -i "s/^\s*nohook wpa_supplicant/#-ap &/" /etc/dhcpcd.conf
    sudo systemctl restart dhcpcd
fi

log " -> COMPLETED"
