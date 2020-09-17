FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"


SRC_URI += "file://0001-Common-sourec-files.patch \
            file://0002-Implement-Heart-Beat-LED.patch \
            file://0003-hwmon-Add-APML-SB-TSI-driver.patch \
            file://0004-hwmon-Add-MCT-OCP-sensors-driver.patch \
            file://0005-irqchip-Add-Aspeed-SCU-Interrupt-Controller.patch \
            file://0006-Support-interrupt-handle-for-aspeed-bmc-misc.patch \
            file://0007-Add-the-mutex-lock-for-aspeed-misc-driver.patch \
            "

