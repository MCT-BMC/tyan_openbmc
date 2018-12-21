#!/bin/bash

echo "Enter Power Reset System Action"

pwrstatus=$(/usr/sbin/gpioutil -n D2 --getval)
if [ $pwrstatus -eq 1 ]; then

    # *** Reset ***
    /usr/sbin/gpioutil -n E3 --setdir out
    /usr/sbin/gpioutil -n E3 --setval 0
    sleep 1
    /usr/sbin/gpioutil -n E3 --setval 1
else
    echo "System is off state."

fi

echo "Exit Power reset System action"
exit 0;
