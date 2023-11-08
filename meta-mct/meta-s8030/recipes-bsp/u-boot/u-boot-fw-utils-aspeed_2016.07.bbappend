FILESEXTRAPATHS_prepend := "${THISDIR}/u-boot-aspeed:"

SRC_URI += "file://0001-Set-the-default-UART-route-setting.patch \
            file://0002-get-mac-from-eeprom.patch \
            file://0003-Set-BMC-RTL8211E-PHY-LED.patch \
            file://0004-Disable-BMC-SPI-multi-Function-pin.patch \
            file://0005-Set-VGA-source-with-AC-lost-status-setting.patch \
            file://0006-Disable-uboot-delay.patch \
            file://0007-Disable-cursor-for-kernel-boot-logo.patch \
            "

