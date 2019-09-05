FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-s7106-disable-Intel-OEM-command.patch \
            file://0002-s7106-force-scanned-fru-as-ID0.patch \
            file://0003-s7106-add-SDR-type-3-support.patch \
            "

