#!/bin/bash

echo "Enter Power on System action"

#USE GPIOD2 to check power status
#pwrstatus=$(/usr/bin/gpioget `gpiofind PS_PWROK`)
pwrstatus=$(busctl get-property org.openbmc.control.Power /org/openbmc/control/power0 org.openbmc.control.Power pgood | cut -d' ' -f2)
if [ $pwrstatus -eq 0 ]; then
    # *** Push power button ***
    # GPIO E1 for power on
    /usr/bin/gpioset `gpiofind PWBTN_OUT`=0
    sleep 2
    /usr/bin/gpioset `gpiofind PWBTN_OUT`=1

fi

echo "Exit Power on System action"
exit 0;
