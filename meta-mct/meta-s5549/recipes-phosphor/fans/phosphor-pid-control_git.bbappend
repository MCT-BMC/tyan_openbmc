FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-remove-the-sesnor-data-scaling.patch \
            file://0002-Fan-speed-control.patch \
            file://0003-fix-Tjmax-change-don-t-take-effect-issue.patch \
            file://0004-Fix-the-pid-coredump-issue.patch \
            file://0005-fix-typo-of-fan-control.patch \
            file://0006-fixed-increase-and-decrease-refresh-time.patch \
            "

