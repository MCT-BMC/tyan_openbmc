FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_prepend = "file://iio-hwmon.conf \
                   file://vsensor/pch-temp.conf"

do_install_append() {
        install -d ${D}/etc/default/obmc/hwmon/
        install -m 644 ${WORKDIR}/iio-hwmon.conf ${D}/etc/default/obmc/hwmon/iio-hwmon.conf
        install -d ${D}/etc/default/obmc/hwmon/vsensor
        install -m 644 ${WORKDIR}/vsensor/pch-temp.conf ${D}/etc/default/obmc/hwmon/vsensor/pch-temp.conf
}

NAMES = " \
        i2c@1e78a000/i2c-bus@40/nct7802@28 \
        pwm-tacho-controller@1e786000 \
        "

ITEMSFMT = "ahb/apb/{0}.conf"

ITEMS = "${@compose_list(d, 'ITEMSFMT', 'NAMES')}"

ENVS = "obmc/hwmon/{0}"
SYSTEMD_ENVIRONMENT_FILE_${PN} += "${@compose_list(d, 'ENVS', 'ITEMS')}"

       