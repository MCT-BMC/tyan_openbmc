FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0002-Add-interface-for-caterr-logging.patch \
            file://0003-Add-SOL-PATTERN-Interface-for-SOL-Processing.patch \
            file://0004-Add-interface-for-AC-restore-always-on-policy.patch \
            file://0005-Add-interface-for-setting-specified-service-status.patch \
            file://0006-Set-watchdog-initialized-flag-true-as-default.patch \
            file://0007-Add-interface-for-DCMI-power.patch \
            "

