#!/bin/sh

echo " Start control UID LED to Blink $1 s"
i=0
count=$1

while [ 1 ]
do
    if [ "$i" -eq "$count" ]; then
        /usr/bin/gpioget gpiochip0 2
        /usr/bin/gpioget gpiochip0 45
        break
    fi

    /usr/bin/gpioset gpiochip0 2=0
    sleep 0.5
    /usr/bin/gpioset gpiochip0 2=1
    sleep 0.5

    i=`expr $i + 1`

done
