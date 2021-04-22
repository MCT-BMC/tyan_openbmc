SUMMARY = "MiTAC BIOS update handler"
DESCRIPTION = "Implement the bios update progress"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

inherit autotools pkgconfig
inherit obmc-phosphor-ipmiprovider-symlink
inherit pythonnative

S = "${WORKDIR}"

SRC_URI = "file://bootstrap.sh \
           file://configure.ac \
           file://bios-update.cpp \
           file://bios-update.hpp \
           file://reserved-address.hpp \
           file://bios-update-handler \
           file://image-active \
           file://LICENSE \
           file://Makefile.am \
          "

DEPENDS += "autoconf-archive-native"
DEPENDS += "sdbusplus sdbusplus-native"
DEPENDS += "phosphor-logging"
DEPENDS += "phosphor-dbus-interfaces phosphor-dbus-interfaces-native"
DEPENDS += "sdbus++-native"
DEPENDS += "boost"
DEPENDS += "phosphor-dbus-interfaces"
DEPENDS += "libgpiod"


RDEPENDS_${PN} += " \
        sdbusplus \
        phosphor-logging \
        phosphor-dbus-interfaces \
        libgpiod \
        "

# PACKAGECONFIG[debug] = "--enable-debug"
# PACKAGECONFIG_append = "debug"

do_install_append() {
    install -d ${D}/usr/sbin
    install -m 0755 ${S}/bios-update-handler ${D}/${sbindir}/
    install -m 0755 ${S}/image-active ${D}/${sbindir}/
}