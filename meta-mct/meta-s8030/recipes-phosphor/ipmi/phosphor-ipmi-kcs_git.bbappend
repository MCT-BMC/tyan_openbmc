FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

KCS_DEVICE = "ipmi-kcs2"
KCS_DEVICE2 = "ipmi-kcs3"

SYSTEMD_SERVICE_${PN} += "${PN}@${KCS_DEVICE2}.service"

SRC_URI += "file://0001-IPMB-support-for-NM-commands.patch"

