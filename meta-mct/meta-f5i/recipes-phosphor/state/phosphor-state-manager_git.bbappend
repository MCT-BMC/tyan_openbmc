FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Detect-host-status-by-pgood.patch \
            file://0002-host_state_manager.cpp-add-warm-reset-feature.patch \
            file://0003-Determine-the-host-initial-state-with-pgood.patch \
           "

