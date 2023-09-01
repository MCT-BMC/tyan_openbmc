DESCRIPTION = "MCT early post feature"
PR = "r1"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

inherit obmc-phosphor-systemd
inherit systemd

S = "${WORKDIR}/"
SRC_URI = "file://mct-early-post.tar.gz \
           file://bootlogo.png \
           file://mct-early-post.service \
           "

DEPENDS += "boost"
DEPENDS += "systemd"
DEPENDS += "cairo"
DEPENDS += "sdbusplus"
DEPENDS += "obmc-libmisc"
DEPENDS += "mct-lib-misc"

RDEPENDS_${PN} += "libsystemd"
RDEPENDS_${PN} += "bash"
RDEPENDS_${PN} += "cairo"
RDEPENDS_${PN} += "sdbusplus"
RDEPENDS_${PN} += "obmc-libmisc"
RDEPENDS_${PN} += "mct-lib-misc"

SYSTEMD_SERVICE_${PN} = "mct-early-post.service"
INSANE_SKIP_${PN} += "already-stripped"

FILES_${PN} += "/usr/share/*"

do_install() {
    install -d ${D}/usr/sbin
    install -m 0755 ${S}/mct-early-post ${D}/${sbindir}/

    install -d ${D}/usr/share/mct-early-post
    install -m 0755 ${S}/bootlogo.png ${D}/${datadir}/mct-early-post/
}
