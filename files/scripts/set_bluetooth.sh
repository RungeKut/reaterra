#!/bin/sh
. ~/scripts/utils.sh
[ "$DEBUG_SCRIPT" = "1" ] && set -x || set +x

log "Bluetooth kernel patch started..."

if ( [ "$#" != "1" ] ) || ( [ "$1" != "orig" ] && [ "$1" != "fake" ] ); then
    echo "Should be one argument: set_bluetooth orig|fake"
    log "Arguments error!"
    return 0
fi

if [ "$1" = "orig" ]; then
    log "Original bluetooth kernel setup..."
    sudo cp ~/others/bluetooth/5.4.73-v7+/no_patch/kernel7.img /boot/
    sudo cp ~/others/bluetooth/5.4.73-v7+/no_patch/bluetooth.txt /boot/
    sudo cp ~/others/bluetooth/5.4.73-v7+/no_patch/drivers/*.* /lib/modules/5.4.73-v7+/kernel/drivers/bluetooth/
    sudo cp ~/others/bluetooth/5.4.73-v7+/no_patch/net/bluetooth*.* /lib/modules/5.4.73-v7+/kernel/net/bluetooth/
    sudo cp ~/others/bluetooth/5.4.73-v7+/no_patch/net/bnep.ko /lib/modules/5.4.73-v7+/kernel/net/bluetooth/bnep/
    sudo cp ~/others/bluetooth/5.4.73-v7+/no_patch/net/hidp.ko /lib/modules/5.4.73-v7+/kernel/net/bluetooth/hidp/
    sudo cp ~/others/bluetooth/5.4.73-v7+/no_patch/net/rfcomm.ko /lib/modules/5.4.73-v7+/kernel/net/bluetooth/rfcomm/
else
    log "Fake bluetooth kernel setup..."	
    sudo cp ~/others/bluetooth/5.4.73-v7+/patched/kernel7.img /boot/
    sudo cp ~/others/bluetooth/5.4.73-v7+/patched/bluetooth.txt /boot/
    sudo cp ~/others/bluetooth/5.4.73-v7+/patched/drivers/*.* /lib/modules/5.4.73-v7+/kernel/drivers/bluetooth/
    sudo cp ~/others/bluetooth/5.4.73-v7+/patched/net/bluetooth*.* /lib/modules/5.4.73-v7+/kernel/net/bluetooth/
    sudo cp ~/others/bluetooth/5.4.73-v7+/patched/net/bnep.ko /lib/modules/5.4.73-v7+/kernel/net/bluetooth/bnep/
    sudo cp ~/others/bluetooth/5.4.73-v7+/patched/net/hidp.ko /lib/modules/5.4.73-v7+/kernel/net/bluetooth/hidp/
    sudo cp ~/others/bluetooth/5.4.73-v7+/patched/net/rfcomm.ko /lib/modules/5.4.73-v7+/kernel/net/bluetooth/rfcomm/
fi

log " -> COMPLETED"

sudo reboot