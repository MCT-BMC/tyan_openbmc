SUMMARY = "CATEER Trigger application"
PR = "r1"

inherit obmc-phosphor-systemd
inherit obmc-phosphor-ipmiprovider-symlink
inherit pythonnative
inherit systemd
inherit obmc-phosphor-systemd
inherit cmake

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${MCTBASE}/COPYING.apache-2.0;md5=34400b68072d710fecd0a2940a0d1658"

DEPENDS += "virtual/obmc-gpio-monitor"
RDEPENDS_${PN} += "virtual/obmc-gpio-monitor"
DEPENDS += "libpeci"

S = "${WORKDIR}/"
SRC_URI += " \
            file://LICENSE \
            file://prochot_thermtrip_update.sh \
            file://xyz.openbmc_project.prochot0.service \
            file://xyz.openbmc_project.prochot0_deassert.service \
            file://xyz.openbmc_project.thermtrip.service \
            "

DEPENDS += "sdbusplus sdbusplus-native"
DEPENDS += "phosphor-logging"
DEPENDS += "phosphor-dbus-interfaces phosphor-dbus-interfaces-native"
DEPENDS += "sdbus++-native"

do_install_append() {
    install -d ${D}/usr/sbin
    install -m 0755 ${WORKDIR}/prochot_thermtrip_update.sh ${D}/${sbindir}/
}

SYSTEMD_ENVIRONMENT_FILE_${PN} +="obmc/gpio/prochot0"
SYSTEMD_ENVIRONMENT_FILE_${PN} +="obmc/gpio/prochot0_deassert"

PROCHOT0_SERVICE = "prochot0"
PROCHOT0_DEASSERT_SERVICE = "prochot0_deassert"


TMPL = "phosphor-gpio-monitor@.service"
INSTFMT = "phosphor-gpio-monitor@{0}.service"
TGT = "${SYSTEMD_DEFAULT_TARGET}"
FMT = "../${TMPL}:${TGT}.requires/${INSTFMT}"

SYSTEMD_SERVICE_${PN} += "xyz.openbmc_project.prochot0.service"
SYSTEMD_SERVICE_${PN} += "xyz.openbmc_project.prochot0_deassert.service"
SYSTEMD_SERVICE_${PN} += "xyz.openbmc_project.thermtrip.service"

SYSTEMD_LINK_${PN} += "${@compose_list(d, 'FMT', 'PROCHOT0_SERVICE')}"
SYSTEMD_LINK_${PN} += "${@compose_list(d, 'FMT', 'PROCHOT0_DEASSERT_SERVICE')}"

# linux-libc-headers guides this way to include custom uapi headers
CFLAGS_append = " -I ${STAGING_KERNEL_DIR}/include/uapi"
CFLAGS_append = " -I ${STAGING_KERNEL_DIR}/include"
CXXFLAGS_append = " -I ${STAGING_KERNEL_DIR}/include/uapi"
CXXFLAGS_append = " -I ${STAGING_KERNEL_DIR}/include"
do_configure[depends] += "virtual/kernel:do_shared_workdir"


