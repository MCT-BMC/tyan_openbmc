#!/usr/bin/env python

import os
import subprocess
import obmc_system_config as System
import time

def main():
    try:
        pch_bus = System.PCH_CONFIG.get('i2c_bus', '')
    except:
        print("There is not PCH_CONFIG with i2c_bus defined in machine.py")
        return 1

    print("Sync BMC time with PCH RTC on I2C bus " + pch_bus+ "...")

    subprocess.call("/usr/bin/timedatectl set-ntp no", shell=True)
    time.sleep(5)
    print("start i2c access")
    try: 
        SEC = subprocess.check_output("/usr/sbin/i2cget -f -y " + pch_bus + " 0x44 0x9 | awk 'BEGIN{FS=\"x\"}{print $2}'", shell=True)
    except: 
        print("access RTC failed")
        return 1
    
    SEC = SEC.strip()
    MIN = subprocess.check_output("/usr/sbin/i2cget -f -y " + pch_bus + " 0x44 0xa | awk 'BEGIN{FS=\"x\"}{print $2}'", shell=True)
    MIN = MIN.strip()
    HOUR = subprocess.check_output("/usr/sbin/i2cget -f -y " + pch_bus + " 0x44 0xb | awk 'BEGIN{FS=\"x\"}{print $2}'", shell=True)
    HOUR = HOUR.strip()
    DAY = subprocess.check_output("/usr/sbin/i2cget -f -y " + pch_bus + " 0x44 0xd | awk 'BEGIN{FS=\"x\"}{print $2}'", shell=True)
    DAY = DAY.strip()
    MON = subprocess.check_output("/usr/sbin/i2cget -f -y " + pch_bus + " 0x44 0xe | awk 'BEGIN{FS=\"x\"}{print $2}'", shell=True)
    MON = MON.strip()
    YEAR = subprocess.check_output("/usr/sbin/i2cget -f -y " + pch_bus + " 0x44 0xf | awk 'BEGIN{FS=\"x\"}{print $2}'", shell=True)
    YEAR = YEAR.strip()
    try: 
        subprocess.call("/usr/bin/timedatectl set-time "+YEAR+"-"+MON+"-"+DAY, shell=True)
    except:
        print("set time failed")
        return 1

    subprocess.call("/usr/bin/timedatectl set-time "+HOUR+":"+MIN+":"+SEC, shell=True)

    print "sync time=20"+YEAR+"-"+MON+"-"+DAY+" "+HOUR+":"+MIN+":"+SEC
    newDate = subprocess.call("/bin/date", shell=True)
    return 0

if __name__ == '__main__':
    main()

# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
