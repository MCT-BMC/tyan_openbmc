FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Implement-the-clear-bios-post-code-feature-for-host-.patch \
            file://0002-Send-dbus-singal-with-received-post-code.patch \
            file://0003-Add-restart-property-for-post-code-manager-service.patch \
            file://0004-Use-binary-serialization-instead-of-JSON.patch \
            file://0005-Add-one-second-timer-to-save-POST-codes-to-file.patch \
           "

