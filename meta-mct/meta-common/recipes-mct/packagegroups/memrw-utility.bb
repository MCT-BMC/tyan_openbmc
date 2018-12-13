LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

FILESEXTRAPATHS_append := "${THISDIR}/files:"

#inherit packagegroup

S = "${WORKDIR}/"

SRC_URI = "file://memrw-utility.tgz"

RDEPENDS_${PN} = "bash"

SYSTEMD_PACKAGES = "${PN}"

INSANE_SKIP_${PN} += "already-stripped"
INSANE_SKIP_${PN} += "ldflags"

#inherit autotools
#deltask do_package

do_install() {
    install -d ${D}/usr/sbin
    install -m 0755 ${S}md ${D}/${sbindir}/
    install -m 0755 ${S}mw ${D}/${sbindir}/
}
