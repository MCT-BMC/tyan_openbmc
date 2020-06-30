FLASH_RWFS_OFFSET = "32768"

OBMC_IMAGE_EXTRA_INSTALL_append_s8036 =   " libgpiod \
                                          gpio-initial \
                                          hbled-ctrl \
                                          s8036-powerctrl \
                                          s8036-default-action \
                                          cpu-state \ 
                                          phosphor-ipmi-ipmb \
                                          entity-manager \
                                          dbus-sensors \
                                          obmc-ikvm \ 
                                          intel-ipmi-oem \
                                          phosphor-sel-logger \
                                          s8036-sol-processing \
                                          s8036-ipmi-oem \
                                          phosphor-pid-control \
                                          id-button \
                                          pwr-button \
                                          bios-update \
                                          s8036-config \
                                          memrw-utility \
                                          system-watchdog \
                                          phosphor-u-boot-mgr \
                                          bmc-sel \
                                          mct-ipmi-oem \
                                          crashdump-bin \
                                          button-monitor \
                                          entity-rescan \
                                          phosphor-post-code-manager \
                                          phosphor-host-postd \
                                          ethtool \
                                          mct-dcmi-power \
                                        "
OBMC_IMAGE_EXTRA_INSTALL_remove = " sync-pch-rtc \
                                    "