log() 
{
    [ "$#" != "1" ] && { echo "(${0##*/}) log arg error - not one argument"; return 1; }  

    dstr="$(date +%Y%m%d_%H%M%S)(${0##*/}) : $1"
    echo "$dstr" >> ~/start_stop_log.txt

    if [ "$CONSOLE_FOR_LOG" = "1" ]; then 
        echo "$dstr"
    fi
}

log2()
{
    log "$@"
    [ "$CMDPATH" != "" ] && echo "$(date +%Y%m%d.%H%M%S): ${@}" >> ${CMDPATH}.log
}


trim()
{
    echo "$(echo "$@" | tr -d '\n\r' )"
}


get3N() 
{
    case $1 in
        [0-9]) echo "00${1}" ;;
        [0-9][0-9]) echo "0${1}" ;;
        [0-9][0-9][0-9]) echo "${1}" ;;
        *) echo "" ;;
    esac
}

get4N() 
{
    case $1 in
        [0-9]) echo "000${1}" ;;
        [0-9][0-9]) echo "00${1}" ;;
        [0-9][0-9][0-9]) echo "0${1}" ;;
        [0-9][0-9][0-9][0-9]) echo "${1}" ;;
        *) echo "" ;;
    esac
}
