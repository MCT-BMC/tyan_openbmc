FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-s7106-hard-coded-patch-for-s7106-FRU-scaning-need-to.patch \
            file://0002-force-bus-3-to-be-8-bit-EEPROM.patch \
            "

SRC_URI += "file://*.json"

do_install_append(){
	install -d ${D}/usr/share/entity-manager/configurations
	install -m 0444 ${WORKDIR}/*.json ${D}/usr/share/entity-manager/configurations
}
