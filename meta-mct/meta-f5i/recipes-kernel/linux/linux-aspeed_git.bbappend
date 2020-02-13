FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

KBRANCH = "dev-5.4"
LINUX_VERSION = "5.4.16"
SRCREV="86f83a82c7052073cba3db4549233e9c0641484b"

SRC_URI += "file://f5i.cfg \
            file://0001-f5i-Initial-dts.patch \
            file://0002-S7106-Common-sourec-files.patch \
            file://0003-Implement-Heart-Beat-LED.patch \
            file://0004-Set-RTD_1_2_3-to-thermistor-mode.patch \
            file://0005-Support-IPMB-in-OpenBMC.patch \
            file://0006-Modified-ibm-cffps-driver.patch \
            file://0007-Fix-fan-sensor-disappear-when-power-off-problem.patch \
            file://0008-s7106-increase-rofs-space.patch \
            "

