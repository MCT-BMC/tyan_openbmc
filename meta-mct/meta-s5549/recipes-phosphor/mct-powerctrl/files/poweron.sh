#!/bin/bash

echo "Enter Power on System action"

for (( i=0; i<=6; i=i+1 ))
do
    duringPowerOff=$(systemctl is-active host-poweroff.service)
    if [ "inactive" != $duringPowerOff ]; then
        echo "During power off: $duringPowerOff"
        sleep 1
    else
        break
    fi
done
pwrstatus=$(busctl get-property org.openbmc.control.Power /org/openbmc/control/power0 org.openbmc.control.Power pgood | cut -d' ' -f2)
if [ $pwrstatus -eq 0 ]; then
    /usr/bin/gpioget `gpiofind PWRBTN_OUT` > /dev/null 
    sleep 1	
    echo "Execute Power on"
    # *** Push power button ***
    # GPIO E1 for power on
    /usr/bin/gpioset `gpiofind PWRBTN_OUT`=0
    sleep 2
    /usr/bin/gpioset `gpiofind PWRBTN_OUT`=1 

fi

echo "Exit Power on System action"
exit 0;
