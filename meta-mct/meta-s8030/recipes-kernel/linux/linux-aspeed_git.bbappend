FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://s8030.cfg \
            file://0001-s8030-initial-dts.patch \
            file://0004-Set-RTD_1_2_3-to-thermistor-mode.patch \
            file://0006-Modified-ibm-cffps-driver.patch \
            file://0007-Fix-fan-sensor-disappear-when-power-off-problem.patch \
            file://0008-s8030-increase-rofs-space.patch \
            "

