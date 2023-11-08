FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

do_install_append() {
    # Disable getty@tty1 from starting at boot time.
    sed -i -e "s/enable getty@.service/disable getty@.service/g" ${D}${systemd_unitdir}/system-preset/90-systemd.preset
}