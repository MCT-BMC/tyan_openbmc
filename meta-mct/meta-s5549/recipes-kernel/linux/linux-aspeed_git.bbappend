FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"


SRC_URI += "file://s5549.cfg \
            file://aspeed-bmc-mct-s5549.dts;subdir=git/arch/${ARCH}/boot/dts \
            file://0004-Set-RTD_1_2_3-to-thermistor-mode.patch \
            file://0006-Modified-ibm-cffps-driver.patch \
            file://0007-Fix-fan-sensor-disappear-when-power-off-problem.patch \
            file://0008-increase-rofs-space.patch \
            file://0009-Register-all-of-dimm-sensors-when-initializing.patch \
            file://0010-change-aspeed-rpm-driver-to-falling-edge.patch \
            file://0011-Enable-pass-through-on-GPIOE1-and-GPIOE3-free.patch \
            file://0012-Enable-GPIOE0-and-GPIOE2-pass-through-by-default.patch \
            file://0013-Allow-monitoring-of-power-control-input-GPIOs.patch \
            "

