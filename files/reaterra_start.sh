#!/bin/sh
. ~/scripts/utils.sh

# Uncomment to debug traces
#export DEBUG_SCRIPT=1

# Uncomment to send messages to console
#export CONSOLE_FOR_LOG=1

[ "$DEBUG_SCRIPT" = "1" ] && set -x || set +x


export SYSTEM_NAME=reaterra

# TURNED_ON pin indicator can be configured here or in /boot/config.txt by gpio-poweroff
# 0 - will turn off control here
TURNED_ON_PIN=0

if [ $TURNED_ON_PIN -ne 0 ]; then
    sudo echo "$TURNED_ON_PIN" > /sys/class/gpio/export
    sleep 0.2
    sudo echo "out" > /sys/class/gpio/gpio$TURNED_ON_PIN/direction
    sudo echo "1" > /sys/class/gpio/gpio$TURNED_ON_PIN/value
fi

sudo echo "44" > /sys/class/gpio/export
sudo echo "out" > /sys/class/gpio/gpio44/direction
sudo echo "1" > /sys/class/gpio/gpio44/value
sudo echo "45" > /sys/class/gpio/export
sudo echo "out" > /sys/class/gpio/gpio45/direction
sudo echo "1" > /sys/class/gpio/gpio45/value

~/scripts/usb_check.sh

while :
do
    log "START"

    ./reaterra 
    RET=$?

    MSG="STOP Retcode: $RET -> Next action:"

    if [ $RET -eq 19 ]; then
        log "$MSG HALT" 

        if [ $TURNED_ON_PIN -ne 0 ]; then
            echo "0" > /sys/class/gpio/gpio$TURNED_ON_PIN/value
	fi

        sudo shutdown -h now
        break
    elif [ $RET -eq 0 ]; then
        log "$MSG EXIT" 
        break
    else
        log "$MSG RESTART" 
    fi
done
