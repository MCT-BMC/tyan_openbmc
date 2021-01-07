FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0002-Add-interface-for-caterr-logging.patch \
            file://0003-Add-SOL-PATTERN-Interface-for-SOL-Processing.patch \
            file://0005-Add-interface-for-setting-specified-service-status.patch \
            file://0006-Set-watchdog-initialized-flag-true-as-default.patch \
            file://0007-Add-interface-for-DCMI-power.patch \
            file://0008-Set-watchdog-initialized-action-to-None-as-default.patch \
            file://0009-Add-the-version-ID-parameter-to-software-version-int.patch \
            file://0010-Add-x86-power-control-relatived-dbus-interface.patch \
            file://0011-Add-property-for-power-soft-action.patch \
            "

