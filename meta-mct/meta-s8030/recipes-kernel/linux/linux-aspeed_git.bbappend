FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://s8030.cfg \
            file://logo_tyan_clut224.ppm;subdir=git/drivers/video/logo \
            file://0001-s8030-initial-dts.patch \
            file://0004-Set-RTD_1_2_3-to-thermistor-mode.patch \
            file://0006-Modified-ibm-cffps-driver.patch \
            file://0007-Fix-fan-sensor-disappear-when-power-off-problem.patch \
            file://0008-s8030-increase-rofs-space.patch \
            file://0009-Revert-Support-JTAG-driver-for-ast2500.patch \
            file://0010-drivers-jtag-Add-Aspeed-SoC-24xx-25xx-26xx-families-.patch \
            file://0011-Support-setting-DAC-source-in-DTS.patch \
            file://0012-Support-default-mode-for-aspeed-gfx-driver.patch \
            file://0013-Filter-same-post-code-with-post-value-zero.patch \
            file://0014-Support-tyan-boot-logo.patch \
            file://0015-Support-set-vga-source-feature-for-aspeed-bmc-misc-d.patch \
            "

