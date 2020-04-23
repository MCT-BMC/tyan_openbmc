FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-s7106-disable-Intel-OEM-command.patch \
            file://0002-Force-baseboard-FRU-to-ID0.patch \
            file://0003-add-SDR-type-3-support.patch \
            file://0009-support-AddSelEntry-command.patch \
            file://0011-Change-ipmi_sel-location-to-persistent-folder.patch \
            file://0013-force-mb-fru-to-256-bytes.patch \
            file://0014-implement-defined-sdr-table.patch \
            file://0016-Support-the-reading-unavailable-bit.patch \
            file://0017-fix-negative-value-issue.patch \
            file://0018-support-leaky-bucket.patch \
            file://0020-support-fru-sync-to-smbios.patch \
            file://0021-workaroud-for-ipmitool-fru-edit-issue.patch \
            "

