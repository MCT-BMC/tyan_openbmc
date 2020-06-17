FILESEXTRAPATHS_prepend_s8036 := "${THISDIR}/${PN}:"

SRC_URI_append_s8036 = " file://ipmb-channels.json"

do_install_append_s8036(){
    install -m 0644 -D ${WORKDIR}/ipmb-channels.json \
                      ${D}/usr/share/ipmbbridge
}

