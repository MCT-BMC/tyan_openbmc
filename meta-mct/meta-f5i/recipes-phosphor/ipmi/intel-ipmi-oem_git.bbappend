FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-s7106-disable-Intel-OEM-command.patch \
            file://0002-Force-baseboard-FRU-to-ID0.patch \
            file://0003-s7106-add-SDR-type-3-support.patch \
            file://0004-s7106-add-ipmi-acpi-support.patch \
            file://0005-Support-IPMI-system-restart-event.patch \
            file://0006-pwr-button-sel-log.patch \
            file://0007-version-change-sel-log.patch \
            file://0008-add-IPMI-ac-lost-event-log.patch \
            file://0009-support-AddSelEntry-command.patch \
            file://0010-BMC-reboot-sel-log.patch \
            file://0011-Change-ipmi_sel-location-to-persistent-folder.patch \
            file://0012-PSU-status-sel-log.patch \
            file://0013-force-mb-fru-to-256-bytes.patch \
            file://0014-implement-defined-sdr-table.patch \
            file://0015-fix-ipmi-get-sel-issue.patch \
            file://0016-Support-the-reading-unavailable-bit.patch \
            file://0017-fix-negative-value-issue.patch \
            file://0018-support-leaky-bucket.patch \
            file://0019-FRU-state-sensor.patch \
            "

