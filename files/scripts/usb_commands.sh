#!/bin/sh
. ~/scripts/utils.sh
[ "$DEBUG_SCRIPT" = "1" ] && set -x || set +x

[ "$#" != "1" ] && { log "arg error - no argument - path to command file directory"; return 1; }

DRIVE=$1



backup()
{
    ~/scripts/start_backup.sh $SYSTEM_NAME

    BACKUP_FILE=$(ls ~/backup_dir/)
    cp ~/backup_dir/*.* "${DRIVE}/" 
    rm -rf ~/backup_dir
    log "Backup file was transfered to USB drive: ${BACKUP_FILE}"
}




export CMDPATH="${DRIVE}/setwifi.txt"
if [ -f "$CMDPATH" ]; then
    log "Set WiFi command received"

    MODE=""
    SSID=""
    KEY=""
    while IFS='=' read -r key value; do
        [ "$key" = "mode" ] && MODE=$(trim ${value})
        [ "$key" = "ssid" ] && SSID=$(trim ${value})
        [ "$key" = "key" ] && KEY=$(trim ${value})
    done < "$CMDPATH"

    mv "$CMDPATH" "${CMDPATH}.done"

    if ( [ "$MODE" = "" ] || [ "$SSID" = "" ] || [ "$KEY" = "" ] ); then
        log2 "MODE or SSID or KEY is empty. Command is canceled."
        return 1
    fi

    ~/scripts/set_wifi.sh ${MODE} ${SSID} ${KEY}
fi

CMDPATH="${DRIVE}/setid.txt"
if [ -f "$CMDPATH" ]; then
    log "Set WiFi Access Point ID command received"

    NEWID=""
    while IFS='=' read -r key value; do
        [ "$key" = "id" ] && NEWID=$(trim ${value})
    done < "$CMDPATH"

    mv "$CMDPATH" "${CMDPATH}.done"

    if [ "$NEWID" = "" ]; then
        log2 "[id] parameter is not found. Command is canceled."
        return 1
    fi

    ~/scripts/set_id.sh ${NEWID}
fi

CMDPATH="${DRIVE}/restore.txt"
if [ -f "$CMDPATH" ]; then
    log "Restore command received"

    RESTFILE=""
    while IFS='=' read -r key value; do
        [ "$key" = "file" ] && RESTFILE=$(trim ${value})
    done < "$CMDPATH"

    mv "$CMDPATH" "${CMDPATH}.done"

    if [ "$RESTFILE" = "" ]; then
        log2 "[file] parameter is not found or empty. Command is canceled."
        return 1
    fi

    if [ ! -f "${DRIVE}/${RESTFILE}" ]; then
        log2 "Restore file is not found. Command is canceled."
        return 1
    fi

    ~/scripts/start_restore.sh "${DRIVE}/${RESTFILE}"
fi

CMDPATH="${DRIVE}/backup.txt"
if [ -f "$CMDPATH" ]; then
    log "Backup command received"

    mv "$CMDPATH" "${CMDPATH}.done"

    backup

fi

CMDPATH="${DRIVE}/update.txt"
if [ -f "$CMDPATH" ]; then
    log "Update command received"

    backup

    UPDFILE=""
    while IFS='=' read -r key value; do
        [ "$key" = "file" ] && UPDFILE=$(trim ${value})
    done < "$CMDPATH"

    mv "$CMDPATH" "${CMDPATH}.done"

    if [ "$UPDFILE" = "" ]; then
        log2 "[file] parameter is not found or empty. Command is canceled."
        return 1
    fi

    if [ ! -f "${DRIVE}/${UPDFILE}" ]; then
        log2 "Update file is not found. Command is canceled."
        return 1
    fi

    ~/scripts/start_update.sh "${DRIVE}/${UPDFILE}"
fi

CMDPATH="${DRIVE}/bt_orig.txt"
if [ -f "$CMDPATH" ]; then
    log "Set original bluetooth kernel command received"

    mv "$CMDPATH" "${CMDPATH}.done"

    ~/scripts/set_bluetooth.sh orig

fi

CMDPATH="${DRIVE}/bt_fake.txt"
if [ -f "$CMDPATH" ]; then
    log "Set fake bluetooth kernel command received"

    mv "$CMDPATH" "${CMDPATH}.done"

    ~/scripts/set_bluetooth.sh fake

fi


return 0


