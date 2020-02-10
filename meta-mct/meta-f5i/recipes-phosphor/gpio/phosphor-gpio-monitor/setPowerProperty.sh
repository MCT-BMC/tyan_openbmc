#!/bin/sh

if [ "$1" == "on" ]; then
    echo "pgood on"
    busctl set-property "org.openbmc.control.Power" "/org/openbmc/control/power0" "org.openbmc.control.Power" "pgood" i 1
    busctl set-property "org.openbmc.control.Power" "/org/openbmc/control/power0" "org.openbmc.control.Power" "state" i 1
    busctl set-property xyz.openbmc_project.State.Host /xyz/openbmc_project/state/host0 xyz.openbmc_project.State.Host RequestedHostTransition s "xyz.openbmc_project.State.Host.Transition.On"
else
    echo "pgood off"
    busctl set-property "org.openbmc.control.Power" "/org/openbmc/control/power0" "org.openbmc.control.Power" "pgood" i 0
    busctl set-property "org.openbmc.control.Power" "/org/openbmc/control/power0" "org.openbmc.control.Power" "state" i 0
    busctl set-property xyz.openbmc_project.State.Host /xyz/openbmc_project/state/host0 xyz.openbmc_project.State.Host RequestedHostTransition s "xyz.openbmc_project.State.Host.Transition.Off"
fi

