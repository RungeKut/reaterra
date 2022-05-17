#!/bin/sh
. ~/scripts/utils.sh
[ "$DEBUG_SCRIPT" = "1" ] && set -x || set +x

log "post_upgrade started..." 

chmod +x reaterra

VERSION="$(cat ~/prev_version.txt)"


if [ ! -f /boot/overlays/goodix_5d-overlay.dtb ]; then
    log "start touchscreen fix..." 
    # Fix touchscreen problem from this commit (https://github.com/raspberrypi/linux/commit/b7d685c0b1bd1b98af0e9c1f5d43769982bdbfb2)
    chmod +x ~/others/goodix_5d-overlay.dtb
    sudo cp ~/others/goodix_5d-overlay.dtb /boot/overlays/
    sudo rm ~/others/goodix_5d-overlay.dtb
    sudo sed -i "/^dtoverlay=goodix$/ c\#dtoverlay=goodix\ndtoverlay=goodix_5d-overlay.dtb" /boot/config.txt
fi

if [ ! -f /etc/wifi/RT2870AP/RT2870AP.txt ]; then
    log "install driver for mt7601u WiFi..." 
    echo "blacklist mt7601u\n" | sudo tee -a /etc/modprobe.d/raspi-blacklist.conf
    sudo chmod 644 /etc/modprobe.d/raspi-blacklist.conf
    sudo mkdir -p /etc/wifi/RT2870AP
    sudo cp ~/others/RT2870AP.txt /etc/wifi/RT2870AP/
    sudo install -m 644 -c ~/others/mt7601Uap.ko /lib/modules/`uname -r`/kernel/drivers/net/wireless/
    sudo rm /etc/modules
    echo "mt7601Uap\n8192eu\ni2c-dev\nloop" | sudo tee /etc/modules
    echo "\ninterface ra0\n    static ip_address=192.168.0.1/24\n    nohook wpa_supplicant\n" | sudo tee -a /etc/dhcpcd.conf
    echo "\ninterface=ra0\n  dhcp-range=192.168.0.2,192.168.0.20,255.255.255.0,24h\n" | sudo tee -a /etc/dnsmasq.conf
    sudo depmod -a
fi

if [ "$VERSION" -lt "119" ]; then
    sudo sed -i "s/^\(ssid=\).*/\1REANET0000/" /etc/hostapd/hostapd.conf
fi

while !(mysqladmin ping)
do
    sleep 1
    log "Waiting for mariadb ..."
done


if [ "$VERSION" -lt "120" ]; then
    log "DB patch for SAS history fields ..."
    mariadb -u reauser -prEa2AeR REATERRA << EOFSQL
        ALTER TABLE \`REATERRA\`.\`История\` 
        ADD COLUMN \`ДлинаШага\` INT NULL DEFAULT NULL AFTER \`Пульс\`,
        ADD COLUMN \`СимметрияЛевая\` INT NULL DEFAULT NULL AFTER \`ДлинаШага\`,
        ADD COLUMN \`СимметрияПравая\` INT NULL DEFAULT NULL AFTER \`СимметрияЛевая\`,
        ADD COLUMN \`РежимСАС\` VARCHAR(45) NULL DEFAULT NULL AFTER \`СимметрияПравая\`;
EOFSQL
    [ $? -ne 0 ] && log "Operation failed"
    log "DB patch is done ..."
fi

if [ "$VERSION" -lt "121" ]; then
    log "mt7601u patch"
    sudo depmod -a
    [ $? -ne 0 ] && log "Operation failed"
fi

if [ "$VERSION" -lt "124" ]; then
    log "Password update"
    username="pi"
    password="raspberry"
    new_ps="LvUnat_FpeHcy7Ah"

    passwd ${username} << EOD
${password}
${new_ps}
${new_ps}
EOD
fi

#if [ "$VERSION" -lt "135" ]; then
#    log "fstab patch to turn off write cache on usb flash drives"
#    sudo sed -i s/"uid=pi,users,noauto,rw,flush,nofail "/"uid=pi,users,noauto,rw,flush,nofail,sync "/g /etc/fstab
#fi


