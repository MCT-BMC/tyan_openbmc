#!/bin/bash

echo "Check the entity from entity manager"

P3V3_AUX=$(busctl get-property xyz.openbmc_project.ADCSensor /xyz/openbmc_project/sensors/voltage/P3V3_AUX xyz.openbmc_project.Sensor.Value Value| cut -d' ' -f2)

PSU0_INPUT_POWER=$(busctl get-property xyz.openbmc_project.PSUSensor /xyz/openbmc_project/sensors/voltage/PSU0_Input_Voltage  xyz.openbmc_project.Sensor.Value Value| cut -d' ' -f2)

if [ -z $P3V3_AUX ] && [ -z $PSU0_INPUT_POWER ]; then
    echo "Rescan for entity manager"
    $(busctl call xyz.openbmc_project.EntityManager /xyz/openbmc_project/EntityManager xyz.openbmc_project.EntityManager ReScan)
fi

exit 0;
