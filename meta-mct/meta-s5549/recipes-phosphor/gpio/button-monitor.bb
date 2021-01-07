DESCRIPTION = "Button Monitor"
PR = "r1"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

inherit obmc-phosphor-systemd
inherit cmake
inherit systemd


S = "${WORKDIR}/"
SRC_URI = "file://power-button-monitor.cpp \
           file://power-button-monitor.hpp \
           file://power-button-monitor.service \
           file://reset-button-monitor.cpp \
           file://reset-button-monitor.hpp \
           file://reset-button-monitor.service \
           file://util.hpp \
           file://CMakeLists.txt \
           "

DEPENDS += "boost"
DEPENDS += "systemd"

RDEPENDS_${PN} += "libsystemd"
RDEPENDS_${PN} += "bash"

SYSTEMD_SERVICE_${PN} += "power-button-monitor.service"
SYSTEMD_SERVICE_${PN} += "reset-button-monitor.service"


do_install() {
    install -d ${D}/usr/sbin
    install -m 0755 power-button-monitor ${D}/${sbindir}/
    install -m 0755 reset-button-monitor ${D}/${sbindir}/
}
