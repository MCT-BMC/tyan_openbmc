#!/bin/bash

default_duty=128

list=`cat /etc/default/obmc/hwmon/ahb/apb/pwm-tacho-controller\@1e786000.conf | grep LABEL | awk -F '=' '{print $2}'`
listarray=($list)

for (( i=0; i<${#listarray[@]}; i++ ));do
    fan_service=`mapper get-service /xyz/openbmc_project/sensors/fan_tach/${listarray[$i]}`
    if [ "$?" = "0" ]; then
       break
    fi
done

fan_interface="xyz.openbmc_project.Control.FanPwm"
property="Target"

for (( i=0; i<${#listarray[@]}; i++ ));do
    fan_object="/xyz/openbmc_project/sensors/fan_tach/${listarray[$i]}"
    busctl set-property $fan_service $fan_object $fan_interface $property 't' $default_duty
done
