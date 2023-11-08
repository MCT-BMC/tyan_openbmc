#!/bin/bash

BUSCTL_CMD="/usr/bin/env busctl"
EARLY_POST_BUS="xyz.openbmc_project.mct.EarlyPost"
EARLY_POST_PATH="/xyz/openbmc_project/mct/EarlyPost"
EARLY_POST_INTERFACE="xyz.openbmc_project.mct.EarlyPost"
EARLY_POST_METHOD="setTyanVgaModeEnable"

while true; do
    dbus-monitor --system "type='signal',interface='org.freedesktop.DBus.Properties',member='PropertiesChanged',arg0namespace='org.openbmc.control.Power'" |
    while read -r sign; do
        case "$sign" in
            *"int32 0"*)  $BUSCTL_CMD call $EARLY_POST_BUS $EARLY_POST_PATH $EARLY_POST_INTERFACE $EARLY_POST_METHOD b 1;;
        esac
    done
done