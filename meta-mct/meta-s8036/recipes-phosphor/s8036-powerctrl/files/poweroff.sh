#!/bin/bash

echo "Enter Power off System action"

#pwrstatus=$(/usr/bin/gpioget `gpiofind PS_PWROK`)
pwrstatus=$(busctl get-property org.openbmc.control.Power /org/openbmc/control/power0 org.openbmc.control.Power pgood | cut -d' ' -f2)
if [ $pwrstatus -eq 1 ]; then   
    # *** Push power button ***
    # GPIO E1
    /usr/bin/gpioset `gpiofind PWBTN_OUT`=0
    for (( i=0; i<=6; i=i+1 ))
    do
        sleep 1
        #pwrstatus=$(/usr/bin/gpioget `gpiofind PS_PWROK`)
        pwrstatus=$(busctl get-property org.openbmc.control.Power /org/openbmc/control/power0 org.openbmc.control.Power pgood | cut -d' ' -f2)
        if [ $pwrstatus -eq 0 ]; then
             break;    
        fi
    done    
    /usr/bin/gpioset `gpiofind PWBTN_OUT`=1

    sleep 1
fi

obmcutil chassisoff

echo "Exit Power off System action"
exit 0;
