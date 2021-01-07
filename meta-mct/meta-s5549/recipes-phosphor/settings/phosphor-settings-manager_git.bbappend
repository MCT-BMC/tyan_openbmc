FILESEXTRAPATHS_append := ":${THISDIR}/${PN}"
SRC_URI_append = " file://processor-state.override.yml \
                   file://sol-pattern.override.yml \
                   file://power-restore-policy.override.yml \
                   file://service-status.override.yml \
                   file://power-cap.override.yml \
                   file://ac-boot.override.yml \
                   file://power-restore-delay.override.yml \
                   "
