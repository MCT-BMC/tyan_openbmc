#!/bin/bash

service="xyz.openbmc_project.State.BMC"
interface="xyz.openbmc_project.State.BMC"
object="/xyz/openbmc_project/state/bmc0"
property="CurrentBMCState"
BMC_state="NotReady"

while [ $BMC_state != "Ready" ]; do
    BMC_state=$(busctl get-property $service $object $interface $property | awk -F'"' '{print $2}' | awk -F'.' '{print $6}')
    sleep 5
done

if [ ! -d "/sys/bus/i2c/devices/14-001f/hwmon" ]; then
    echo " Didn't have switch port1 OCP card"
    echo 0x1f > /sys/bus/i2c/devices/i2c-14/delete_device

    pservice="xyz.openbmc_project.State.Chassis"
    pinterface="xyz.openbmc_project.State.Chassis"
    pobject="/xyz/openbmc_project/state/chassis0"
    pproperty="CurrentPowerState"
    Power_state="Off"

    # check switch port4 OCP card (Intel-V710) when system power on
    while [ $Power_state != "On" ]; do
        Power_state=$(busctl get-property $pservice $pobject $pinterface $pproperty | awk -F'"' '{print $2}' | awk -F'.' '{print $6}')
        sleep 5
    done

    if [ ! -d "/sys/bus/i2c/devices/17-001f/hwmon" ]; then
        echo " Dir .../17-001f/hwmon does not exists , Re-scan port4 OCP card "
        echo 0x1f > /sys/bus/i2c/devices/i2c-17/delete_device
        echo tmp75 0x1f > /sys/bus/i2c/devices/i2c-17/new_device

        if [ ! -d "/sys/bus/i2c/devices/17-001f/hwmon" ]; then
            echo 0x51 > /sys/bus/i2c/devices/i2c-17/delete_device
            echo 0x1f > /sys/bus/i2c/devices/i2c-17/delete_device
            /usr/bin/gpioset gpiochip0 49=1
            echo " Both OCP card does not exists , disable SW2 "
        else
            echo " switch port4 OCP card exists , re-scan FruDevice"
            systemctl restart xyz.openbmc_project.FruDevice.service
        fi
    fi
else
    echo 0x51 > /sys/bus/i2c/devices/i2c-17/delete_device
    echo 0x1f > /sys/bus/i2c/devices/i2c-17/delete_device
    echo "Didn't have switch port4 OCP card"
fi
