FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://s5549-Baseboard.json \
            file://Chicony-R550-PSU.json \
            file://Delta-DPS-500AB-PSU.json \
            file://Delta-DPS-550AB-PSU.json \
            file://Delta-DPS-650AB-PSU.json \
            file://Common-PSU.json \
            file://blacklist.json \
            file://0001-Support-baseboard-fru-reading.patch \
            file://0004-fix-fsc-to-settable-interface.patch \
            file://0005-remove-NVME-P4000.json.patch \
            file://0006-Set-the-EntityManager-service-booting-after-FruDevic.patch \
            "

do_install_append(){
	install -d ${D}/usr/share/entity-manager/configurations
	install -m 0444 ${WORKDIR}/*.json ${D}/usr/share/entity-manager/configurations
	install -m 0444 ${WORKDIR}/blacklist.json ${D}/usr/share/entity-manager
}
