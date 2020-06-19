SUMMARY = "GPIO initialize"
DESCRIPTION = "Implement the initialization for GPIO"
PR = "r1"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

FILESEXTRAPATHS_append := "${THISDIR}/files:"

inherit autotools pkgconfig
inherit systemd
inherit obmc-phosphor-systemd

S = "${WORKDIR}/"

SRC_URI = "file://bootstrap.sh \
           file://configure.ac \
           file://Makefile.am \
           file://gpio-initial.cpp \
           file://gpio-initial.hpp \
           file://gpio-initial-config.json \
           file://gpio-initial.sh \
           file://gpio-initial.service \
           file://gpio-initial-sysfs.service \
           "

DEPENDS = "systemd"
DEPENDS += "autoconf-archive-native"
DEPENDS += "libgpiod"
DEPENDS += "nlohmann-json"

RDEPENDS_${PN} = "bash"
RDEPENDS_${PN} = "libgpiod"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = "gpio-initial.service"
SYSTEMD_SERVICE_${PN} = "gpio-initial-sysfs.service"

FILES_${PN} += "${datadir}/gpio-initial/*"

do_install() {
    autotools_do_install

    install -d ${D}/usr/sbin
    install -d ${D}/usr/share/gpio-initial
    install -m 0755 ${S}gpio-initial.sh ${D}/${sbindir}/
    install -m 0755 ${S}gpio-initial-config.json ${D}/${datadir}/gpio-initial/
}
