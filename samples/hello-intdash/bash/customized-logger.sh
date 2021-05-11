#!/usr/bin/env bash

set -e


DATA="Hello intdash!"
ID="abc"

FIFO_LGR2MNG="/var/run/intdash/logger_100.tx"
FIFO_MNG2LGR="/var/run/intdash/logger_100.rx"
BINARY_MSG=/var/run/intdash/msg


#
# Send 10x2 times at a frequency of 500 [msec].
#

for COUNT in $(seq 1 20)
do
    # Make binary message for String format
    #
    # +---------------+---------------+---------------+---------------+
    # |    Type(01)   |                    Legnth                     |
    # +---------------+---------------+---------------+---------------+
    # |                           Time Sec                            |
    # +---------------+---------------+---------------+---------------+
    # |                           Time NSec                           |
    # +---------------+---------------+---------------+---------------+
    # |     DType     |      Seq      |   ID Length   |      ID 
    # +---------------+---------------+---------------+-----------//--+
    # |   Data String
    # +-----------//--+

    #
    # Type must be 01
    #

    # Make binary msg
    echo -en "\x01" >${BINARY_MSG}

    #
    # Length from Time Sec to End of Data String
    #

    # Get Data String length
    DATA_LEN=${#DATA}

    # Get ID length
    ID_LEN=${#ID}

    # Calc Length
    MSG_LEN=$(( 4 + 4 + 1 + 1 + 1 + ${ID_LEN} + ${DATA_LEN} ))
    
    # Change data length to hex string
    H_MSG_LEN=$(printf "%06X" ${MSG_LEN})

    # Append binary msg
    echo -en "\x${H_MSG_LEN:4:2}\x${H_MSG_LEN:2:2}\x${H_MSG_LEN:0:2}" >>${BINARY_MSG}

    #
    # Time Sec
    #

    # Get CLOCK_MONOTONIC
    NOW=$(awk '/^now/ {print $3; exit}' /proc/timer_list)

    # Extract sec
    SEC=${NOW:0:-9}

    # Change Time to hex string
    H_SEC=$(printf "%08X" ${SEC})

    # Append binary msg
    echo -en "\x${H_SEC:6:2}\x${H_SEC:4:2}\x${H_SEC:2:2}\x${H_SEC:0:2}" >>${BINARY_MSG}

    #
    # Time Nano Sec
    #

    # Extract nsec
    NSEC=$(expr 0 + ${NOW:${#SEC}:9} )

    # Change Time to hex string
    H_NSEC=$(printf "%08X" ${NSEC})

    # Append binary msg
    echo -en "\x${H_NSEC:6:2}\x${H_NSEC:4:2}\x${H_NSEC:2:2}\x${H_NSEC:0:2}" >>${BINARY_MSG}

    #
    # DType of String must be 0x1d
    #

    # Append binary msg
    echo -en "\x1d" >>${BINARY_MSG}

    #
    # Seq must be 0
    #

    # Append binary msg
    echo -en "\x00" >>${BINARY_MSG}

    #
    # ID Length
    #

    # Change Time to hex string
    H_ID_LEN=$(printf "%02X" ${ID_LEN})

    # Append binary msg
    echo -en "\x${H_ID_LEN}" >>${BINARY_MSG}

    #
    # ID
    #

    # Append binary msg
    echo -n ${ID} >>${BINARY_MSG}

    #
    # Data String
    #

    # Append binary msg
    echo -n ${DATA} >>${BINARY_MSG}


    #
    # Write binary msg to FIFO
    #

    cat ${BINARY_MSG} >>${FIFO_LGR2MNG}

    sleep 0.5
done

#
# Waiting for signal
#
while true
do
    sleep 0.5
done
