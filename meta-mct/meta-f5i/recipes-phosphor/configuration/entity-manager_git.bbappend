FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-s7106-Support-baseboard-fru-reading.patch \
            file://s7106-Baseboard.json \
            file://0002-NVME-change-to-use-NVME-MI-smbus-address.patch \
            file://0003-support-fru-write-offset.patch \
            "

SRC_URI += "file://*.json"

do_install_append(){
	install -d ${D}/usr/share/entity-manager/configurations
	install -m 0444 ${WORKDIR}/*.json ${D}/usr/share/entity-manager/configurations
}
