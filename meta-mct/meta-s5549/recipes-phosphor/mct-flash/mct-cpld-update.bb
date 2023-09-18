LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

FILESEXTRAPATHS_append := "${THISDIR}/files:"

S = "${WORKDIR}/"

SRC_URI = " \
           file://cpld-update \
           file://flash-progress-update \
           "

RDEPENDS_${PN} = "bash"

do_install() {
    install -d ${D}/usr/sbin
    install -m 0755 ${S}cpld-update ${D}/${sbindir}/
    install -m 0755 ${S}flash-progress-update ${D}/${sbindir}/
}
