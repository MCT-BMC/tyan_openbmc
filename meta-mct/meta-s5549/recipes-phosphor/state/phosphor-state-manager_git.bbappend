FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Detect-host-status-by-pgood.patch \
            file://0002-Determine-the-host-initial-state-with-pgood.patch \
            file://0003-Implement-the-AC-restore-random-and-fixed-delay.patch \
            file://0004-ignore-power-policy-when-BMC-reset.patch \
           "

