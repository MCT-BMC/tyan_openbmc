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
           file://configure.ac \
           file://pch_temp_sensor.cpp \
           file://LICENSE \
           file://Makefile.am \
		   file://xyz.openbmc_project.Ipmi.Ipmb.Pchtemp.service \
		   file://xyz.openbmc_project.Hwmon-Pchtemp.service \
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

SYSTEMD_SERVICE_${PN} = "xyz.openbmc_project.Ipmi.Ipmb.Pchtemp.service"
SYSTEMD_SERVICE_${PN} += "xyz.openbmc_project.Hwmon-Pchtemp.service"
