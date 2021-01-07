FILESEXTRAPATHS_prepend_s5549 := "${THISDIR}/${PN}:"
OBMC_CONSOLE_HOST_TTY = "ttyS2"

SRC_URI += "file://obmc-console-ssh@.service \
            file://0001-Support-start-preparing-and-stop-commnad-configurati.patch \
            file://0002-Add-timestamps-to-SOL-buffer.patch \
            "

