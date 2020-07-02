FLASH_RWFS_OFFSET = "32768"

OBMC_IMAGE_EXTRA_INSTALL_append_s8030 = " libgpiod \
                                          gpio-initial \
                                          s8030-powerctrl \
                                          s8030-default-action \
                                          cpu-state \ 
                                          phosphor-ipmi-ipmb \
                                          entity-manager \
                                          dbus-sensors \
                                          obmc-ikvm \ 
                                          intel-ipmi-oem \
                                          phosphor-sel-logger \
                                          s7106-sol-processing \
                                          s7106-ipmi-oem \
                                          phosphor-pid-control \
                                          pwr-button \
                                          bios-update \
                                          s8030-config \
                                          memrw-utility \
                                          system-watchdog \
                                          phosphor-u-boot-mgr \
                                          bmc-sel \
                                          mct-ipmi-oem \
                                          button-monitor \
                                          entity-rescan \
                                          phosphor-post-code-manager \
                                          phosphor-host-postd \
                                          ethtool \
                                          mct-dcmi-power \
                                        "
