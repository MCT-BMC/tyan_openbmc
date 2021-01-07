FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://s5549-Baseboard.json \
            file://Common-PSU.json \
            file://blacklist.json \
            file://0001-Support-baseboard-fru-reading.patch \
            file://0004-fix-fsc-to-settable-interface.patch \
            file://0005-remove-NVME-P4000.json.patch \
            file://0006-Set-the-EntityManager-service-booting-after-FruDevic.patch \
            file://0007-Fix-fru-disappear-after-power-off.patch \
            "

do_install_append(){
	install -d ${D}/usr/share/entity-manager/configurations
	install -m 0444 ${WORKDIR}/*.json ${D}/usr/share/entity-manager/configurations
	install -m 0444 ${WORKDIR}/blacklist.json ${D}/usr/share/entity-manager
}
