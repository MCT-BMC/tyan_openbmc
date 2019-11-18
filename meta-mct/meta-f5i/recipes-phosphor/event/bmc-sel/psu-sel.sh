#!/bin/bash

SendLog=0
Fault=0
Warning=0
SERVICE="xyz.openbmc_project.Logging.IPMI"
OBJECT="/xyz/openbmc_project/Logging/IPMI"
INTERFACE="xyz.openbmc_project.Logging.IPMI"
METHOD="IpmiSelAdd"

while true; do

    StatusWord=`i2cget -f -y 3 0x58 0x79 w`

    #check VOUT fault or warning
    if [ $(((StatusWord & 0x8000) >> 15)) -eq 1 ]; then
        # read STATUS_VOUT 0x7a , check fault or warning
        Status_Vout=`i2cget -f -y 3 0x58 0x7a`
        if [ $((Status_Vout & 0x94)) -gt 0 ]; then
            Fault=1
        else
            Warning=1
        fi
    fi

    # check IOUT/POUT fault or warning
    if [ $(((StatusWord & 0x4000) >> 14)) -eq 1 ]; then
        # read STATUS_IOUT 0x7b , check fault or warning
        Status_Iout=`i2cget -f -y 3 0x58 0x7b`
        if [ $((Status_Iout & 0xd2)) -gt 0 ]; then
            Fault=1
        else
            Warning=1
        fi
    fi
    
    # check VIN fault or warning
    if [ $(((StatusWord & 0x2000) >> 13)) -eq 1 ]; then
        # read STATUS_INPUT 0x7c , check fault or warning
        Status_Input=`i2cget -f -y 3 0x58 0x7c`
        if [ $((Status_Input & 0x94)) -gt 0 ]; then
            Fault=1
        else
            Warning=1
        fi
    fi
    
    #check FAN fault or warning
    if [ $(((StatusWord & 0x400) >> 10)) -eq 1 ]; then
        # read STATUS_FANS_1_2 0x81 , check fault or warning
        Status_Fan12=`i2cget -f -y 3 0x58 0x81`
        if [ $((Status_Fan12 & 0xc2)) -gt 0 ]; then
            Fault=1
        else
            Warning=1
        fi
    fi
    
    #check TEMPERATURE fault or warning
    if [ $(((StatusWord & 0x4) >> 2)) -eq 1 ]; then
        # read STATUS_TEMPERATURE 0x7d , check fault or warning
        Status_Temp=`i2cget -f -y 3 0x58 0x7d`
        if [ $((Status_Temp & 0x90)) -gt 0 ]; then
            Fault=1
        else
            Warning=1
        fi
    fi
    
    if [ $SendLog == 0 ]; then
        if [ $Fault == 1 ]; then
            # send sel log , 01h : Power Supply Failure detected
            busctl call $SERVICE $OBJECT $INTERFACE $METHOD ssaybq "SEL Entry" "/xyz/openbmc_project/sensors/power_supply/PSU_STATUA" 3 {0x01,0x00,0x00} yes 0x20
            SendLog=1
        elif [ $Warning == 1 ]; then
            # send sel log , 02h : Predictive Failure
            busctl call $SERVICE $OBJECT $INTERFACE $METHOD ssaybq "SEL Entry" "/xyz/openbmc_project/sensors/power_supply/PSU_STATUA" 3 {0x02,0x00,0x00} yes 0x20
            SendLog=1
        fi
    else
        if [ $((StatusWord & 0xe404)) == 0 ]; then
            Fault=0
            Warning=0
            SendLog=0
        fi
    fi
    sleep 1
done
