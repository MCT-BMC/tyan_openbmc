FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Support-temp2-and-share-the-same-threshold.patch \
            file://0002-s7106-add-supported-pmbus-name-cffps1.patch \
            file://0003-s7106-add-cpu-and-dimm-sesnor-via-ME.patch \
            file://0004-Add-gpio-dbus-sensor-for-caterr-feature.patch \
            file://0005-s7106-ipmbsensor-add-DIMM-temperature-channl-4-7-sup.patch \
            file://0006-s7106-filter-out-FAN-threshold-event-during-power-tr.patch \
            file://0007-s7106-new-processor-sensor-to-replace-gpio-sensor.patch \
            file://0008-s7106-add-ipmi-ACPI-sensor.patch \
            "

#SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.gpiosensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.processorsensor.service"
SYSTEMD_SERVICE_${PN} += " xyz.openbmc_project.acpisensor.service"
