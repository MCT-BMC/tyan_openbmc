FILESEXTRAPATHS_prepend_s5549 := "${THISDIR}/${PN}:"

SRC_URI += "file://sdr.json"
FILES_${PN} += "${datadir}/ipmi-providers/sdr.json"

do_install_append() {
    install -m 0644 -D ${WORKDIR}/sdr.json \
    ${D}${datadir}/ipmi-providers/sdr.json
}




