FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

# Disable the CPU sensor Tcontrol threshold setting
EXTRA_OECMAKE_append += " -DBMC_CPU_SENSOR_TCONTROL=OFF"

SRC_URI += "file://0001-add-supported-pmbus-name-cffps1.patch \
            file://0002-Filter-out-FAN-threshold-event-during-power-transiti.patch \
            file://0003-s7106-new-processor-sensor-to-replace-gpio-sensor.patch \
            file://0004-support-ipmi-ACPI-sensor.patch \
            file://0005-add-event-sensor.patch \
            file://0006-s7106-add-OCP-NIC-temperature-sensor.patch \
            file://0007-add-necessary-event-sensors-in-dbus.patch \
            file://0013-add-ipmi-power-status-sensor.patch \
            file://0015-hidden-peci-core-temperature-sensor.patch \
            file://0018-change-power-match-to-pgood-event.patch \
            file://0020-Add-prochot-sensor.patch \
            file://0021-Add-Invalid-Value-property-for-power-on-sensors.patch \
            file://0022-Add-type-sensors-to-support-the-invalid-value-proper.patch \
            file://0023-Add-thermtrip-sensor.patch \
            file://0024-disable-PSU-PWM-detecting.patch \
            file://0025-Add-VR-MOS-Margin-sensor.patch \
            file://0026-support-leaky-bucket-sensor.patch \
            file://0030-add-sensor-reading-unavailable-bit-for-VR-sensor.patch \
            file://0031-Fix-the-cpu-sensor-display.patch \
            file://0032-mct-NVMe-sensor.patch \
            file://0033-Modify-MNVMe-sensor-debug-message-with-using-debug-f.patch \
            file://0034-Fix-cpu-senor-data-update-error-after-code-base-upda.patch \
            file://0035-Fix-CPU-DTS-margin-reading-incorrect.patch \
            "

SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.processorsensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.acpisensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.aclostsensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.mnvmesensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.mostempsensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.hostmemoryeccsensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.eventsensor.service"

