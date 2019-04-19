#!/bin/bash

default_duty=128

echo $default_duty > /sys/class/hwmon/hwmon0/pwm1
echo $default_duty > /sys/class/hwmon/hwmon0/pwm2
echo $default_duty > /sys/class/hwmon/hwmon0/pwm3
echo $default_duty > /sys/class/hwmon/hwmon0/pwm4
echo $default_duty > /sys/class/hwmon/hwmon0/pwm5
