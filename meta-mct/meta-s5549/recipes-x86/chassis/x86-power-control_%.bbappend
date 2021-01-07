FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI = "git://github.com/openbmc/x86-power-control.git"
SRCREV = "6c09007572f6d7ff9aae0acf3f6ebde693b04a75"

DEPENDS += "nlohmann-json"
DEPENDS += "phosphor-dbus-interfaces"

SRC_URI += "file://power-config-host0.json;subdir=git/power-control-x86/config \
            file://0001-Implement-some-power-feature.patch \
            "

SYSTEMD_SERVICE_${PN} += "chassis-system-reset.service \
                          chassis-system-reset.target \
                         "
