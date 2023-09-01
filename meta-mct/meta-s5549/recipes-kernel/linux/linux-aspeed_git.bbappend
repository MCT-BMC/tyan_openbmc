FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"


SRC_URI += "file://s5549.cfg \
            file://logo_tyan_clut224.ppm;subdir=git/drivers/video/logo \
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
            file://0014-Clear-the-i2c-slave-state-when-a-bmc-i2c-master-cmd-.patch \
            file://0015-misc-Add-clock-control-logic-into-Aspeed-LPC-SNOOP-d.patch \
            file://0016-media-aspeed-fix-clock-handling-logic.patch \
            file://0017-Support-setting-DAC-source-in-DTS.patch \
            file://0018-Support-default-mode-for-aspeed-gfx-driver.patch \
            file://0019-Filter-same-post-code-with-post-value-zero.patch \
            file://0020-Support-tyan-boot-logo.patch \
            file://0021-Support-set-vga-source-feature-for-aspeed-bmc-misc-d.patch \
            "

