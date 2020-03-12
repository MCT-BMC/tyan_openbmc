FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Detect-host-status-by-pgood.patch \
            file://0002-Determine-the-host-initial-state-with-pgood.patch \
           "

