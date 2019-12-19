SUMMARY = "MiTAC OEM IPMI commands"
DESCRIPTION = "MiTAC OEM Commands"
PR = "r1"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

S = "${WORKDIR}/"

SRC_URI = "file://CMakeLists.txt\
           file://LICENSE \
           file://oemcmd.cpp \
	       file://oemcmd.hpp "

DEPENDS = "boost phosphor-ipmi-host phosphor-logging systemd intel-dbus-interfaces"
DEPENDS += "libgpiod"

inherit cmake obmc-phosphor-ipmiprovider-symlink
EXTRA_OECMAKE="-DYOCTO=1"


LIBRARY_NAMES = "libmctoemcmds.so"

FILES_${PN}_append = " ${libdir}/ipmid-providers/lib*${SOLIBS}"
FILES_${PN}_append = " ${libdir}/host-ipmid/lib*${SOLIBS}"
FILES_${PN}_append = " ${libdir}/net-ipmid/lib*${SOLIBS}"
FILES_${PN}-dev_append = " ${libdir}/ipmid-providers/lib*${SOLIBSDEV}"

