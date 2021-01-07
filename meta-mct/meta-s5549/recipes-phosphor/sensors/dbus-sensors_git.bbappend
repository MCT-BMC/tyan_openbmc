FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

# Disable the CPU sensor Tcontrol threshold setting
EXTRA_OECMAKE_append += " -DBMC_CPU_SENSOR_TCONTROL=OFF"

SRC_URI += "file://0001-add-supported-pmbus-name-cffps1.patch \
            file://0002-Filter-out-FAN-threshold-event-during-power-transiti.patch \
            file://0003-New-processor-sensor-to-replace-gpio-sensor.patch \
            file://0004-support-ipmi-ACPI-sensor.patch \
            file://0005-add-event-sensor.patch \
            file://0006-Add-OCP-NIC-temperature-sensor.patch \
            file://0007-add-necessary-event-sensors-in-dbus.patch \
            file://0013-add-ipmi-power-status-sensor.patch \
            file://0015-hidden-peci-core-temperature-sensor.patch \
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
            file://0036-Fix-sensor-hysteresis-issue.patch \
            file://0037-Fix-cpu-sensor-hang-and-get-power-status-error.patch \
            file://0038-Add-watchdog2-and-power-control-event-sensor.patch \
            file://0039-Add-DCMI-power-event-sensor.patch \
            file://0040-change-ADC-range-to-be-configurable.patch \
            file://0041-add-assert-value-to-log-event-data.patch \
            file://0042-Stop-the-timer-when-the-deasserted-events-occur.patch \
            file://0043-add-offset-value-for-E810-OCP-card.patch \
            file://0044-log-assert-event-when-timer-cancelled.patch \
            file://0045-filter-out-invalidate-ADC-value.patch \
            file://0047-Filter-out-invalidate-MOS-temperature-value-and-add-.patch \
            file://0046-add-BMC-watchdog-timeout-SEL.patch \
            file://0048-Add-event-disabled-event-sensor.patch \
            file://0049-filter-out-invalidate-sys-air-inlet-value.patch \
            file://0050-Update-BMC-watchdog-timeout-SEL.patch \
            file://0051-Implement-the-average-mode-for-fan-tech-value.patch \
            file://0052-Change-the-using-timer-from-deadline_timer-to-steady.patch \
            file://0053-add-ADC-boundary-retry.patch \
            file://0054-Implement-the-average-mode-for-PSU-fan-sensor-value.patch \
            file://0055-Add-the-debug-log-for-dimm-sensor.patch \
            file://0056-create-bmc-reset-flag.patch \
            "

SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.processorsensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.acpisensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.aclostsensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.mnvmesensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.mostempsensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.hostmemoryeccsensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.eventsensor.service"

DEPENDS += "obmc-libmisc"

RDEPENDS_${PN} += "obmc-libmisc"



