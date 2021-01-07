FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRCREV = "07bb095158b39cedb49dae0972e489a6a2776faf"

SRC_URI += "file://0001-Revert-Remove-HMAC-SHA1-from-Authentication-Integrit.patch \
            file://0002-Add-the-max-size-limit-feature-to-SOL-Console-data.patch \
            "
