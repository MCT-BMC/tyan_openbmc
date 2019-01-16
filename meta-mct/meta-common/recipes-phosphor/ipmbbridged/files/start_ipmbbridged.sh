#!/bin/bash

parentPath="/sys/bus/i2c/devices"
declare -i index=0

for d in $parentPath/* ; do
    name=`cat $d/name`
	if [ "$name" == "slave-mqueue" ];then
		slave_mqueue[index++]=$d
	fi
done

printf '%s\n' "${slave_mqueue[@]}	"

for qpath in ${slave_mqueue[@]} ;do
	echo $qpath"/slave-mqueue"
	qpath="${qpath//-/\\x2d}"
	qpath="${qpath//:/\\x3a}"
	systemctl start 'xyz.openbmc_project.Ipmi.Channel.Ipmb@'$qpath'.service'
done

