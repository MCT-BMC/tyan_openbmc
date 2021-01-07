KCS_DEVICE = "ipmi-kcs2"
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-IPMB-support-for-NM-commands.patch \
            "

