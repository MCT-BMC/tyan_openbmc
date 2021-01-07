#!/bin/bash

echo " Start Check NetIpmid service"
error=0

while true; do

    BMC_IP=$(ipmitool lan print | awk '/^IP Address  /{print $4}')

    if [ "$BMC_IP" != "0.0.0.0" ]; then

        # get device ID via out-band ipmi command
        DeviceID_rep=$(ipmitool -I lanplus -H $BMC_IP -U root -P root raw 6 1)

        if [ -z "$DeviceID_rep" ]; then

            for (( i=0; i<5; i++))
            do
                DeviceID_rep=$(ipmitool -I lanplus -H $BMC_IP -U root -P root raw 6 1)
                if [ -z "$DeviceID_rep" ]; then
                    (( error++ ))
                fi
            done
            
            # check error >= 5 , re-start phosphor-ipmi-net@.service
            if [ $error -ge 5 ]; then
                echo " Re-Start phosphor-ipmi-net service"
                systemctl restart phosphor-ipmi-net@eth0.service
                error=0
                sleep 10
            fi
        else
            error=0
            sleep 10
        fi
    else
        error=0
        sleep 10
    fi
done
