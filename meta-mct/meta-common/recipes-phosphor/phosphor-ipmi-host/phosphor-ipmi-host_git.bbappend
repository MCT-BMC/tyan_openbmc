FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

# *************************************************************
# MiTAC add phosphor-ipmi-host.service
# The phosphor-ipmi-host.service will be replace by this file. 
# This file fixed ordering cycle issue.
# *************************************************************

SRC_URI += "file://0001-Implement-master-write-read-command.patch \
            file://0002-MCT-Common-Bug-10583-The-Firmware-Revision-from-ipmi.patch \
            file://0003-Bug-Fix-watchdog-timer-not-stop-issue-in-AMI-BIOS.patch \
            file://0004-MCT-Common-Fix-incorrect-Subnet-Mask-with-IPMI-Get-Lan.patch \
            file://phosphor-ipmi-host.service \
            "

