FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Light-BMC-Heartbeat-LED.patch \
            file://0002-Set-the-default-UART-route-setting.patch \
            file://0003-get-mac-from-eeprom.patch \
            file://0004-Set-BMC-RTL8211E-PHY-LED.patch \
            file://0006-Set-the-default-GPIO-pass-through.patch \
            "

