#!/bin/bash

echo "Enter Power Reset System Action"

pwrstatus=$(/usr/bin/gpioget gpiochip0 26)
if [ $pwrstatus -eq 1 ]; then

    # *** Reset ***
    /usr/bin/gpioset gpiochip0 35=0
    sleep 1
    /usr/bin/gpioset gpiochip0 35=1
else
    echo "System is off state."

fi

echo "Exit Power reset System action"
exit 0;
