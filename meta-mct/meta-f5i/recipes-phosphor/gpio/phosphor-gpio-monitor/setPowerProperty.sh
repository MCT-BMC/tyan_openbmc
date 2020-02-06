#!/bin/sh

if [ "$1" == "on" ]; then
    echo "pgood on"
    busctl set-property "org.openbmc.control.Power" "/org/openbmc/control/power0" "org.openbmc.control.Power" "pgood" i 1
    busctl set-property xyz.openbmc_project.State.Host /xyz/openbmc_project/state/host0 xyz.openbmc_project.State.Host CurrentHostState s "xyz.openbmc_project.State.Host.HostState.Running"
    busctl set-property xyz.openbmc_project.State.Chassis /xyz/openbmc_project/state/chassis0 xyz.openbmc_project.State.Chassis CurrentPowerState s "xyz.openbmc_project.State.Chassis.PowerState.On"
else
    echo "pgood off"
    busctl set-property "org.openbmc.control.Power" "/org/openbmc/control/power0" "org.openbmc.control.Power" "pgood" i 0
    busctl set-property xyz.openbmc_project.State.Host /xyz/openbmc_project/state/host0 xyz.openbmc_project.State.Host CurrentHostState s "xyz.openbmc_project.State.Host.HostState.Off"
    busctl set-property xyz.openbmc_project.State.Chassis /xyz/openbmc_project/state/chassis0 xyz.openbmc_project.State.Chassis CurrentPowerState s "xyz.openbmc_project.State.Chassis.PowerState.Off"
fi

