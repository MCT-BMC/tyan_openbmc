SUMMARY = "ipmbbridged"
DESCRIPTION = "ipmbbridged from Intel"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

inherit autotools pkgconfig
inherit obmc-phosphor-ipmiprovider-symlink
inherit pythonnative
inherit systemd
inherit obmc-phosphor-systemd

S = "${WORKDIR}/"

SRC_URI = "file://bootstrap.sh \
           file://ipmbbridged.cpp \
           file://ipmbdefines.hpp \
           file://ipmbutils.hpp \
           file://MAINTAINERS \
           file://README.md \
           file://configure.ac \
           file://ipmbbridged.hpp \
           file://ipmbutils.cpp \
           file://LICENSE \
           file://Makefile.am \
           file://xyz.openbmc_project.Ipmi.Channel.Ipmb.conf \
           file://i2c-dev-user.h \
           file://i2c-dev.h \
           file://compiler.h \
           file://compiler_types.h \
		   file://xyz.openbmc_project.Ipmi.Channel.Ipmb@.service \
		   file://start_ipmbbridged.sh \
		   file://ipmb.bridged.start.service \
          "

DEPENDS += "autoconf-archive-native"
DEPENDS += "sdbusplus sdbusplus-native"
DEPENDS += "phosphor-logging"
DEPENDS += "phosphor-dbus-interfaces phosphor-dbus-interfaces-native"
DEPENDS += "sdbus++-native"

RDEPENDS_${PN} += " \
        sdbusplus \
        phosphor-logging \
        phosphor-dbus-interfaces \
        "

SYSTEMD_SERVICE_${PN} = "xyz.openbmc_project.Ipmi.Channel.Ipmb@.service ipmb.bridged.start.service"

RDEPENDS_${PN} = "bash"

do_install() {
    install -d ${D}/usr/sbin
    install -m 0755 ${S}start_ipmbbridged.sh ${D}/${sbindir}/
    install -m 0755 ${S}/build/ipmbbridged ${D}/${sbindir}/
}
