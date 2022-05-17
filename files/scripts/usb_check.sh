#!/bin/sh
. ~/scripts/utils.sh
[ "$DEBUG_SCRIPT" = "1" ] && { echo "set"; set -x; } || { echo "unset"; set +x; }

DRIVE=""
rm -rf /media/usb1/*.*
mount /media/usb1
if [ $? -eq 0 ]; then
    if [ `ls /media/usb1 | wc -l` -gt 0 ]; then
        DRIVE="/media/usb1"
    fi
fi
if [ "$DRIVE" = "" ]; then
    rm -rf /media/usb/*.*
    mount /media/usb
    if [ $? -eq 0 ]; then
        if [ `ls /media/usb | wc -l` -gt 0 ]; then
            DRIVE="/media/usb"
        fi
    fi
fi

#   !!!!!!!!!!!! TEST MODE  !!!!!!!!!!!
#DRIVE="~/"

if [ "$DRIVE" != "" ]; then
    log "USB drive found -> $DRIVE"
    ~/scripts/usb_commands.sh $DRIVE
    umount ${DRIVE}                     
fi