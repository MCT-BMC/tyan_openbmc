FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Remove-smbioshandler.cpp-fro-CMake-config-to-avoid-b.patch"
SRC_URI += "file://0002-s7106-force-scanned-fru-as-ID0.patch"

