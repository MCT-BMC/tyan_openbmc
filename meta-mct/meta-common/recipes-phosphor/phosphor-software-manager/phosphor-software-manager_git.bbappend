FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Allow-force-update-the-same-version-image.patch \
            file://0002-Implement-the-progress-for-updating-BIOS-image-to-ho.patch \
            file://0003-Add-support-for-Cpld-update.patch \
            "

PACKAGECONFIG[flash_cpld] = "--enable-cpld_upgrade"

SYSTEMD_SERVICE_${PN}-updater += "${@bb.utils.contains('PACKAGECONFIG', 'flash_cpld', 'obmc-flash-cpld@.service', '', d)}"

PACKAGECONFIG_append = "flash_bios"