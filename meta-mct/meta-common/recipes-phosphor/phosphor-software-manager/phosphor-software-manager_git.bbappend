FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Allow-force-update-the-same-version-image.patch \
            file://0002-Implement-the-progress-for-updating-BIOS-image-to-ho.patch \
            "

PACKAGECONFIG_append = "flash_bios"