DESCRIPTION = "Entity rescan"
PR = "r1"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

inherit systemd
inherit obmc-phosphor-systemd

S = "${WORKDIR}/"
SRC_URI = "file://entity_rescan.sh \
           file://entity-rescan.service \
           "

DEPENDS = "systemd"
RDEPENDS_${PN} = "bash"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = "entity-rescan.service"

do_install() {
    install -d ${D}/usr/sbin
    install -m 0755 ${S}entity_rescan.sh ${D}/${sbindir}/
}
