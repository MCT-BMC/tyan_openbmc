FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

#Enable threshold monitoring
EXTRA_OECMAKE += "-DSEL_LOGGER_MONITOR_THRESHOLD_EVENTS=ON"
EXTRA_OECMAKE += "-DREDFISH_LOG_MONITOR_PULSE_EVENTS=ON"
#SRC_URI += "file://0001-Add-CATERR-sel-log-monitor.patch"

SRC_URI += "file://0001-Change-ipmi_sel-location-to-persistent-folder.patch \
            file://0002-f5i-alert-led-when-happen-warning.patch \
            "

