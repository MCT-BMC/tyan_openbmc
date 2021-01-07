SUMMARY = "Power related trigger application"
PR = "r1"

inherit obmc-phosphor-systemd

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

DEPENDS += "virtual/obmc-gpio-monitor"
RDEPENDS_${PN} += "virtual/obmc-gpio-monitor"

S = "${WORKDIR}"
SRC_URI += "file://setPowerProperty.sh \
            file://SetPowerGoodPropertyOff.service \
            file://SetPowerGoodPropertyOn.service \
            "

SYSTEMD_SERVICE_${PN} += "SetPowerGoodPropertyOff.service"
SYSTEMD_SERVICE_${PN} += "SetPowerGoodPropertyOn.service"

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${WORKDIR}/setPowerProperty.sh ${D}/${sbindir}/setPowerProperty.sh
}
