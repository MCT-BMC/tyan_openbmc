FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

inherit obmc-phosphor-systemd

SRC_URI = "git://github.com/openbmc/phosphor-pid-control.git"
SRCREV = "a7ec8350d17b70153cebe666d3fbe88bddd02a1a"

SRC_URI += "file://0001-remove-the-sesnor-data-scaling.patch \
            file://0002-Fan-speed-control.patch \
            file://0003-fix-Tjmax-change-don-t-take-effect-issue.patch \
            file://0004-Fix-the-pid-coredump-issue.patch \
            file://0005-fix-typo-of-fan-control.patch \
            file://0006-fixed-increase-and-decrease-refresh-time.patch \
            "

SYSTEMD_SERVICE_${PN} = "phosphor-pid-control.service"

EXTRA_OECONF = "--enable-configure-dbus=yes"

FILES_${PN} = "${bindir}/swampd ${bindir}/setsensor"
