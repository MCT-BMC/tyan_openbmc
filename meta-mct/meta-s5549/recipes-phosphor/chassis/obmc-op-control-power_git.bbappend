FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

S = "${WORKDIR}/git/op-pwrctl/"

SRC_URI += "file://0001-leave-gpood-monitor-to-other-service.patch"

