FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-s7106-Support-baseboard-fru-reading.patch \
            file://s8030-Baseboard.json \
            file://blacklist.json \
            file://0004-fix-fsc-to-settable-interface.patch \
            file://0005-remove-NVME-P4000.json.patch \
            file://0006-Set-the-EntityManager-service-booting-after-FruDevic.patch \
            "

SRC_URI += "file://*.json"

do_install_append(){
	install -d ${D}/usr/share/entity-manager/configurations
	install -m 0444 ${WORKDIR}/*.json ${D}/usr/share/entity-manager/configurations
	install -m 0444 ${WORKDIR}/blacklist.json ${D}/usr/share/entity-manager
}
