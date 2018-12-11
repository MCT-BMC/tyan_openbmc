SUMMARY = "udisks provides dbus interfaces for disks and storage devices"
LICENSE = "GPLv2+ & LGPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=dd79f6dbbffdbc8e86b086a8f0c0ef43"

DEPENDS = " \
    acl \
    libatasmart \
    polkit \
    libgudev \
    dbus-glib \
    glib-2.0 \
    libblockdev \
    intltool-native \
    gnome-common-native \
    libxslt-native \
"
DEPENDS += "${@bb.utils.filter('DISTRO_FEATURES', 'systemd', d)}"

RDEPENDS_${PN} = "acl"

SRC_URI = " \
    git://github.com/storaged-project/udisks.git \
    file://0001-data-fix-out-of-tree-build.patch \
    file://non-gnu-libc.patch \
"
SRCREV = "b6471f2e99f6aa1133af0de658f1fa05c748932c"
S = "${WORKDIR}/git"

CVE_PRODUCT = "udisks"

inherit autotools systemd gtk-doc gobject-introspection

EXTRA_OECONF = "--disable-man --disable-gtk-doc"

FILES_${PN} += " \
    ${datadir}/dbus-1/ \
    ${datadir}/polkit-1 \
    ${datadir}/bash-completion \
    ${libdir}/polkit-1/extensions/*.so \
    ${nonarch_base_libdir}/udev/* \
    ${exec_prefix}${nonarch_base_libdir}/udisks2/* \
    ${systemd_system_unitdir} \
"

PACKAGES =+ "${PN}-libs"
FILES_${PN}-libs = "${libdir}/lib*${SOLIBS}"

SYSTEMD_SERVICE_${PN} = "${BPN}.service"
SYSTEMD_AUTO_ENABLE = "disable"
