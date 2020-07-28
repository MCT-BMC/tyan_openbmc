FILESEXTRAPATHS_prepend_s8030 := "${THISDIR}/${PN}:"

SRC_URI_append_s8030 = " file://ipmb-channels.json"

do_install_append_s8030(){
    install -m 0644 -D ${WORKDIR}/ipmb-channels.json \
                      ${D}/usr/share/ipmbbridge
}

