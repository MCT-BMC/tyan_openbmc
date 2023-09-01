SUMMARY = "MiTAC MISC Library"
DESCRIPTION = "MiTAC MISC Library."
PR = "r1"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

DEPENDS += "systemd \
            glib-2.0 \
            boost \
            nlohmann-json \
            "
RDEPENDS_${PN} += "libsystemd \
                   glib-2.0 \
                   boost \
                   "
TARGET_CC_ARCH += "${LDFLAGS}"

S = "${WORKDIR}"

SRC_URI = "file://libmisc.cpp \
           file://libmisc.hpp \
           file://Makefile \
           file://COPYING.MIT \
          "

do_install() {
        install -d ${D}${libdir}
        install -Dm755 libmctmisc.so ${D}${libdir}/libmctmisc.so
        install -d ${D}${includedir}/mct
        install -m 0644 ${S}/libmisc.hpp ${D}${includedir}/mct/libmisc.hpp
}

FILES_${PN} = "${libdir}/libmctmisc.so"
FILES_${PN}-dev = "${includedir}/mct/"

