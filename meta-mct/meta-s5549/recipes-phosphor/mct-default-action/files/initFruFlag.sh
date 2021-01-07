#! /bin/sh

echo " Init Fru Flag & Get Fru Data "

$(echo 1 > /usr/sbin/fruFlag)
command=`ipmitool fru print 0`
$(echo $command > /usr/sbin/fruData)

