FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://phosphor-multi-gpio-monitor.service"
SRC_URI += "file://phosphor-gpio-monitor@.service"
SRC_URI += "file://s8036_gpio.json"
SRC_URI += "file://0001-Support-GPIO-interrupt.patch \
            file://0002-init-host-status.patch \
            file://SetPowerGoodPropertyOff.service \
            file://SetPowerGoodPropertyOn.service \
            file://setPowerProperty.sh \
           "

FILES_${PN}-monitor += "${datadir}/phosphor-gpio-monitor/phosphor-multi-gpio-monitor.json"
FILES_${PN}-monitor += "${sbindir}/setPowerProperty.sh"

SYSTEMD_SERVICE_${PN}-monitor += "phosphor-multi-gpio-monitor.service"
SYSTEMD_SERVICE_${PN}-monitor += "SetPowerGoodPropertyOff.service"
SYSTEMD_SERVICE_${PN}-monitor += "SetPowerGoodPropertyOn.service"

do_install_append(){
    install -d ${D}/usr/share/phosphor-gpio-monitor
    install -m 0444 ${WORKDIR}/*.json ${D}/usr/share/phosphor-gpio-monitor/phosphor-multi-gpio-monitor.json
    install -d ${D}/usr/sbin
    install -m 0755 ${WORKDIR}/setPowerProperty.sh ${D}/${sbindir}/
}

