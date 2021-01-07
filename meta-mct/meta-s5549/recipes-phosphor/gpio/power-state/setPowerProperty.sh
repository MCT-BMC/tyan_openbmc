#!/bin/sh

bmcState=$(busctl get-property xyz.openbmc_project.State.BMC /xyz/openbmc_project/state/bmc0 xyz.openbmc_project.State.BMC CurrentBMCState|cut -d ' ' -f 2|cut -d . -f 6|cut -c -5)

#offstatus=$(systemctl is-active host-poweroff.service)

if [ "$1" == "on" ]; then
    echo "pgood on"
    busctl set-property "org.openbmc.control.Power" "/org/openbmc/control/power0" "org.openbmc.control.Power" "pgood" i 1
    busctl set-property "org.openbmc.control.Power" "/org/openbmc/control/power0" "org.openbmc.control.Power" "state" i 1
    if [ ! -d "/run/openbmc/" ]; then
        mkdir /run/openbmc/
    fi
    touch /run/openbmc/chassis@0-on
    touch /run/openbmc/host@0-on
        	
    if [ -f "/run/openbmc/ipmi-poweron" ]; then 
	rm /run/openbmc/ipmi-poweron
    else
        echo "set host to on"

        busctl set-property xyz.openbmc_project.State.Host /xyz/openbmc_project/state/host0 xyz.openbmc_project.State.Host CurrentHostState s "xyz.openbmc_project.State.Host.HostState.Running"
        busctl set-property xyz.openbmc_project.State.Host /xyz/openbmc_project/state/host0 xyz.openbmc_project.State.Host RequestedHostTransition s "xyz.openbmc_project.State.Host.Transition.On"
        # set chassis to on
        busctl set-property xyz.openbmc_project.State.Chassis /xyz/openbmc_project/state/chassis0 xyz.openbmc_project.State.Chassis CurrentPowerState s "xyz.openbmc_project.State.Chassis.PowerState.On"
        busctl set-property xyz.openbmc_project.State.Chassis /xyz/openbmc_project/state/chassis0 xyz.openbmc_project.State.Chassis RequestedPowerTransition s "xyz.openbmc_project.State.Chassis.Transition.On"
    fi

else
    echo "pgood off"
    busctl set-property "org.openbmc.control.Power" "/org/openbmc/control/power0" "org.openbmc.control.Power" "pgood" i 0
    busctl set-property "org.openbmc.control.Power" "/org/openbmc/control/power0" "org.openbmc.control.Power" "state" i 0
	rm -f /run/openbmc/chassis@0-on
	rm -f /run/openbmc/host@0-on
	rm -f /run/openbmc/host@0-request
    
    if [ -f "/run/openbmc/ipmi-poweroff" ]; then
	rm /run/openbmc/ipmi-poweroff
    else
        echo "set host to off"
	busctl set-property xyz.openbmc_project.State.Host /xyz/openbmc_project/state/host0 xyz.openbmc_project.State.Host CurrentHostState s "xyz.openbmc_project.State.Host.HostState.Off"
        busctl set-property xyz.openbmc_project.State.Host /xyz/openbmc_project/state/host0 xyz.openbmc_project.State.Host RequestedHostTransition s "xyz.openbmc_project.State.Host.Transition.Off"
        busctl set-property xyz.openbmc_project.State.Chassis /xyz/openbmc_project/state/chassis0 xyz.openbmc_project.State.Chassis CurrentPowerState s "xyz.openbmc_project.State.Chassis.PowerState.Off"
        busctl set-property xyz.openbmc_project.State.Chassis /xyz/openbmc_project/state/chassis0 xyz.openbmc_project.State.Chassis RequestedPowerTransition s "xyz.openbmc_project.State.Chassis.Transition.Off"
    fi
fi

