LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

FILESEXTRAPATHS_append := "${THISDIR}/files:"

inherit systemd
inherit obmc-phosphor-systemd

S = "${WORKDIR}/"

SRC_URI = " \
           file://initFruFlag.service \
           file://initFruFlag.sh \
           file://writeFRU.sh \
           file://relinkLan.sh \
           "

DEPENDS = "systemd"
RDEPENDS_${PN} = "bash"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = " \
                         initFruFlag.service \
                         "


do_install() {
    install -d ${D}/usr/sbin
    install -m 0755 ${S}initFruFlag.sh ${D}/${sbindir}/
    install -m 0755 ${S}writeFRU.sh ${D}/${sbindir}/
    install -m 0755 ${S}relinkLan.sh ${D}/${sbindir}/
}
