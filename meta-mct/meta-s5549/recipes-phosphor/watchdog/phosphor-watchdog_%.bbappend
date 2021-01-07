FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Fix-watchdog-timeout-action-do-not-work.patch \
            file://0002-Implement-add-SEL-feature-for-watchdog-timeout.patch \
            "

