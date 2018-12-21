#!/bin/bash

echo "Enter Power on System action"

pwrstatus=$(/usr/sbin/gpioutil -n D2 --getval)
if [ $pwrstatus -eq 0 ]; then
    # *** Push power button ***
    GPIO_BASE=$(cat /sys/devices/platform/ahb/ahb:apb/1e780000.gpio/gpio/*/base)
    # GPIO E1
    GPIO_NUM=$(($GPIO_BASE + 32 + 1))
    
    echo ${GPIO_NUM} > /sys/class/gpio/export

    echo "out" > /sys/class/gpio/gpio${GPIO_NUM}/direction
    echo 0 > /sys/class/gpio/gpio${GPIO_NUM}/value
    sleep 2
    echo 1 > /sys/class/gpio/gpio${GPIO_NUM}/value
    
    echo ${GPIO_NUM} > /sys/class/gpio/unexport
fi

echo "Exit Power on System action"
exit 0;
