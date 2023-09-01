FILESEXTRAPATHS_prepend := "${THISDIR}/u-boot-aspeed:"

SRC_URI += "file://0001-Light-BMC-Heartbeat-LED.patch \
            file://0002-Set-the-default-UART-route-setting.patch \
            file://0003-get-mac-from-eeprom.patch \
            file://0004-Set-BMC-RTL8211E-PHY-LED.patch \
            file://0006-Set-the-default-GPIO-pass-through.patch \
            file://0007-Enable-WDT-in-uboot.patch \
            file://0008-Set-VGA-source-with-AC-lost-status-setting.patch \
            file://0009-Disable-uboot-delay.patch \
            file://0010-Disable-cursor-for-kernel-boot-logo.patch \
            "
