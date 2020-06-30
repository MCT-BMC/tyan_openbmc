#!/usr/bin/env python

import os
import subprocess

def main():
    res = subprocess.call("/usr/bin/timedatectl set-ntp no", shell=True)
    print res

    # Inform other services time sync is done. 
    os.mknod("/run/time_sync_done")
    return 0

if __name__ == '__main__':
    main()

# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
