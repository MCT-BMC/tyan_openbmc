FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Set-the-default-UART-route-setting.patch \
            file://0002-get-mac-from-eeprom.patch \
            file://0003-Set-BMC-RTL8211E-PHY-LED.patch \
            file://0004-Disable-BMC-SPI-multi-Function-pin.patch \
            "

