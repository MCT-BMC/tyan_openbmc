SUMMARY = "MiTAC environment manager"
DESCRIPTION = "Implement the manager for environment"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

S = "${WORKDIR}"

SRC_URI = "file://mct-default-env.conf \
           "

FILES_${PN} += "${systemd_unitdir}/system.conf.d"

do_install() {
    install -m 0755 -d ${D}${systemd_unitdir}/system.conf.d
    install -m 0755 ${WORKDIR}/mct-default-env.conf ${D}${systemd_unitdir}/system.conf.d/90-mct-default-env.conf
}
