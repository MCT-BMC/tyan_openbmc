FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-add-SEL-log-for-Power-Button-Press.patch \
            file://0002-Add-Power-button-override-SEL.patch \
            file://0003-Remove-power-button-and-reset-button-press-and-long-.patch \
            file://0004-Write-to-SEL-when-reset-button-is-pressed.patch \
            file://0005-Fix-SEL-event-issue-for-the-power-and-reset-button.patch \
            "

