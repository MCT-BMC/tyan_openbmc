SUMMARY = "Post code description json configuration"
DESCRIPTION = "Implement the container for post code description json configuration"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

FILESEXTRAPATHS_append := "${THISDIR}/${PN}:"

S = "${WORKDIR}"

SRC_URI = "file://s5549-post-code-config.json \
           "

do_install() {
    install -d ${D}/usr/share/mct-post-code-config
    install -m 0755 ${S}/s5549-post-code-config.json ${D}/${datadir}/mct-post-code-config/
}
