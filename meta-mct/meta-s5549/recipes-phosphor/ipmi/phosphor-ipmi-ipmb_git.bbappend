FILESEXTRAPATHS_prepend_s5549 := "${THISDIR}/${PN}:"

SRC_URI_append_s5549 = " file://ipmb-channels.json"

do_install_append_s5549(){
    install -m 0644 -D ${WORKDIR}/ipmb-channels.json \
                      ${D}/usr/share/ipmbbridge
}

