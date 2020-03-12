FILESEXTRAPATHS_prepend_f5i := "${THISDIR}/${PN}:"

SRC_URI_append_f5i = " file://ipmb-channels.json"

do_install_append_f5i(){
    install -m 0644 -D ${WORKDIR}/ipmb-channels.json \
                      ${D}/usr/share/ipmbbridge
}

