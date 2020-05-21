FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0004-Implement-NMI-command-in-Chassis-Control-command.patch \
            file://0005-f5i-ipmi-move-Unspecified-ip-source-from-static-to-D.patch \
            file://0006-add-power-diag-log.patch \
            file://0007-fix-set-static-ip-fail-issue.patch \
            file://0008-F5i-add-ARP-control-status.patch \
            file://0009-change-ipmit-power-reset-from-dc-cycle-to-reset.patch \
            file://0010-Fix-BIOS-boot-type-setting-in-system-boot-option-fea.patch \
            file://0011-fix-warm-reset-command.patch \
            file://0012-Fix-expiration-flags-unexpected-status-in-get-watchd.patch \
            "
EXTRA_OECONF += "--disable-i2c-whitelist-check"

