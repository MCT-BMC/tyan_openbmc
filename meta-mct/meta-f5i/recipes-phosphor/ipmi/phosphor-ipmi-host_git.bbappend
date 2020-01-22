FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-support-ipmi-chassis-reset-command.patch \
            file://0003-Implement-master-write-read-command.patch \
            file://0004-Implement-NMI-command-in-Chassis-Control-command.patch \
            file://0005-f5i-ipmi-move-Unspecified-ip-source-from-static-to-D.patch \
            file://0006-add-power-diag-log.patch \
            "


