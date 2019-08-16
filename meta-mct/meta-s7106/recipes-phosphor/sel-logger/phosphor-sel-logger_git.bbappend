FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

#Enable threshold monitoring
EXTRA_OECMAKE += "-DSEL_LOGGER_MONITOR_THRESHOLD_EVENTS=ON"

#SRC_URI += "file://0001-Add-CATERR-sel-log-monitor.patch"

