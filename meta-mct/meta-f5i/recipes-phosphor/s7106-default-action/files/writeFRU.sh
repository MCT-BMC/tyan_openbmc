#! /bin/sh

echo " Start Write Fru Data "

i=0

while read line
do
    str[$i]=$line
    data=`ipmitool fru edit 0 field ${str[$i]}`

    sleep 3
    i=`expr $i + 1`
done < /usr/sbin/fruWrite

`rm -rf /usr/sbin/fruWrite`

