SUMMARY = "CATEER Trigger application"
PR = "r1"

inherit obmc-phosphor-systemd
inherit autotools pkgconfig
inherit obmc-phosphor-ipmiprovider-symlink
inherit pythonnative
inherit systemd
inherit obmc-phosphor-systemd


LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

DEPENDS += "virtual/obmc-gpio-monitor"
RDEPENDS_${PN} += "virtual/obmc-gpio-monitor"

S = "${WORKDIR}"
SRC_URI += "file://bootstrap.sh \
            file://configure.ac \
            file://caterr_update.cpp \
            file://LICENSE \
            file://Makefile.am \
            file://caterr_update.sh \
            file://prochot_thermtrip_update.sh \
            file://xyz.openbmc_project.caterr.service \
            file://xyz.openbmc_project.caterr_deassert.service \
            file://xyz.openbmc_project.prochot0.service \
            file://xyz.openbmc_project.prochot0_deassert.service \
            file://xyz.openbmc_project.prochot1.service \
            file://xyz.openbmc_project.prochot1_deassert.service \
            file://xyz.openbmc_project.thermtrip.service \
            file://xyz.openbmc_project.thermtrip_deassert.service "

DEPENDS += "autoconf-archive-native"
DEPENDS += "sdbusplus sdbusplus-native"
DEPENDS += "phosphor-logging"
DEPENDS += "phosphor-dbus-interfaces phosphor-dbus-interfaces-native"
DEPENDS += "sdbus++-native"

do_install() {
    install -d ${D}/usr/sbin
    install -m 0755 ${WORKDIR}/caterr_update.sh ${D}/${sbindir}/
    install -m 0755 ${WORKDIR}/prochot_thermtrip_update.sh ${D}/${sbindir}/
}

SYSTEMD_ENVIRONMENT_FILE_${PN} +="obmc/gpio/caterr"
SYSTEMD_ENVIRONMENT_FILE_${PN} +="obmc/gpio/caterr_deassert"
SYSTEMD_ENVIRONMENT_FILE_${PN} +="obmc/gpio/prochot0"
SYSTEMD_ENVIRONMENT_FILE_${PN} +="obmc/gpio/prochot0_deassert"
SYSTEMD_ENVIRONMENT_FILE_${PN} +="obmc/gpio/prochot1"
SYSTEMD_ENVIRONMENT_FILE_${PN} +="obmc/gpio/prochot1_deassert"
SYSTEMD_ENVIRONMENT_FILE_${PN} +="obmc/gpio/thermtrip"
SYSTEMD_ENVIRONMENT_FILE_${PN} +="obmc/gpio/thermtrip_deassert"

CATERR_SERVICE = "caterr"
CATERR_DEASSERT_SERVICE = "caterr_deassert"
PROCHOT0_SERVICE = "prochot0"
PROCHOT0_DEASSERT_SERVICE = "prochot0_deassert"
PROCHOT1_SERVICE = "prochot1"
PROCHOT1_DEASSERT_SERVICE = "prochot1_deassert"
THERMTRIP_SERVICE = "thermtrip"
THERMTRIP_DEASSERT_SERVICE = "thermtrip_deassert"


TMPL = "phosphor-gpio-monitor@.service"
INSTFMT = "phosphor-gpio-monitor@{0}.service"
TGT = "${SYSTEMD_DEFAULT_TARGET}"
FMT = "../${TMPL}:${TGT}.requires/${INSTFMT}"

SYSTEMD_SERVICE_${PN} += "xyz.openbmc_project.caterr.service"
SYSTEMD_SERVICE_${PN} += "xyz.openbmc_project.caterr_deassert.service"
SYSTEMD_SERVICE_${PN} += "xyz.openbmc_project.prochot0.service"
SYSTEMD_SERVICE_${PN} += "xyz.openbmc_project.prochot0_deassert.service"
SYSTEMD_SERVICE_${PN} += "xyz.openbmc_project.prochot1.service"
SYSTEMD_SERVICE_${PN} += "xyz.openbmc_project.prochot1_deassert.service"
SYSTEMD_SERVICE_${PN} += "xyz.openbmc_project.thermtrip.service"

SYSTEMD_LINK_${PN} += "${@compose_list(d, 'FMT', 'CATERR_SERVICE')}"
SYSTEMD_LINK_${PN} += "${@compose_list(d, 'FMT', 'CATERR_DEASSERT_SERVICE')}"
SYSTEMD_LINK_${PN} += "${@compose_list(d, 'FMT', 'PROCHOT0_SERVICE')}"
SYSTEMD_LINK_${PN} += "${@compose_list(d, 'FMT', 'PROCHOT0_DEASSERT_SERVICE')}"
SYSTEMD_LINK_${PN} += "${@compose_list(d, 'FMT', 'PROCHOT1_SERVICE')}"
SYSTEMD_LINK_${PN} += "${@compose_list(d, 'FMT', 'PROCHOT1_DEASSERT_SERVICE')}"



