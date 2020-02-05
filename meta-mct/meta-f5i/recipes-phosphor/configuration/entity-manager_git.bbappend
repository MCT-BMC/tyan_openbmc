FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-s7106-Support-baseboard-fru-reading.patch \
            file://s7106-Baseboard.json \
            file://0004-fix-fsc-to-settable-interface.patch \
            file://0005-remove-NVME-P4000.json.patch \
            "

SRC_URI += "file://*.json"

do_install_append(){
	install -d ${D}/usr/share/entity-manager/configurations
	install -m 0444 ${WORKDIR}/*.json ${D}/usr/share/entity-manager/configurations
}
