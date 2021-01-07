FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0004-Implement-NMI-command-in-Chassis-Control-command.patch \
            file://0005-ipmi-move-Unspecified-ip-source-from-static-to-D.patch \
            file://0006-add-power-diag-log.patch \
            file://0007-fix-set-static-ip-fail-issue.patch \
            file://0008-add-ARP-control-status.patch \
            file://0009-change-ipmit-power-reset-from-dc-cycle-to-reset.patch \
            file://0010-Fix-BIOS-boot-type-setting-in-system-boot-option-fea.patch \
            file://0011-fix-warm-reset-command.patch \
            file://0012-Fix-expiration-flags-unexpected-status-in-get-watchd.patch \
            file://0013-Implement-power-control-SEL-generated-in-chassis-con.patch \
            file://0014-Implement-the-DCMI-power-reading-and-power-limit-com.patch \
            file://0016-rewrite-Get-Session-Info-to-use-new-yielding-D-Bus-A.patch \
            file://0017-Fix-the-wrong-field-value-for-session-info-commnad.patch \
            file://0018-Add-the-detect-for-firmare-revision.patch \
            file://0020-Change-power-soft-command-from-action-off-to-action-.patch \
            "
EXTRA_OECONF += "--disable-i2c-whitelist-check"

