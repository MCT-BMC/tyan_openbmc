FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"


SRC_URI += "file://s5549.cfg \
            file://aspeed-bmc-mct-s5549.dts;subdir=git/arch/${ARCH}/boot/dts \
            file://0004-Set-RTD_1_2_3-to-thermistor-mode.patch \
            file://0006-Modified-ibm-cffps-driver.patch \
            file://0007-Fix-fan-sensor-disappear-when-power-off-problem.patch \
            file://0008-increase-rofs-space.patch \
            file://0009-Register-all-of-dimm-sensors-when-initializing.patch \
            file://0010-change-aspeed-rpm-driver-to-falling-edge.patch \
            "

