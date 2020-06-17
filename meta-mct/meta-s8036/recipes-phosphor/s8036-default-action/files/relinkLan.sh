#! /bin/sh

echo " Start Re-link Lan Port "
`ip link set eth0 down`
`ip link set eth0 up`
