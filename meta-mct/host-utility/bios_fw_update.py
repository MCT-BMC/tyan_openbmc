#!/usr/bin/env python
"""
Usage: bios_fw_update.py 
                                --file <bin-file> 
                                --bmc <bmc-ip> 
                                [-v]

This scripts copies bios bin file to BMC,
and uses REST APIs to update the bin file to BIOS via bmc.
"""

import argparse
import json
import os
import sys
import subprocess
import tarfile
from subprocess import check_output,CalledProcessError
from time import sleep

utilityVersion = '0.03'

prepareProcessing = 'Wating System Power off'
prepareFinish = 'System status is Off , Switch SPI interface'
switchSPIFinish = 'Prepare Update BIOS'
writingFlash = 'Writing images to flash'
finishFlash = 'Apply Complete.  Reboot to take effect.'
failedFlash = 'Apply failed'
restoreFinish = 'Restore From Update setting'

updateTimeout = 600
jsonPrepareUpdate = 0
jsonStatus = 1

def deleteLastLine():
    # cursor up one line
    sys.stdout.write('\x1b[1A')
    # delete last line
    sys.stdout.write('\x1b[2K')

def checkBmcAlive():
    url = DestUrl+'/xyz/openbmc_project/'
    cmds = ['curl', '-k', url]
    print(cmds)
    try:
        output = subprocess.check_output(cmds, stderr=FNULL)
    except CalledProcessError as e:
        # Sometimes curl fails with timeout error, let's ignore it
        return ''
    if FNULL is None:  # Do not print log when FNULL is devnull
        print output
    
    try:
        status = json.loads(output)['status']    
    except ValueError:
        print output
        return False
    
    if status == 'ok':
        return True
    else:
        return False

def getUpdateJsonData():
    url = DestUrl+'/org/openbmc/control/flash/bios'
    cmds = ['curl', '-k', url]
    # print(cmds)
    try:
        output = subprocess.check_output(cmds, stderr=FNULL)
    except CalledProcessError as e:
        # Sometimes curl fails with timeout error, let's ignore it
        return ''
    if FNULL is None:  # Do not print log when FNULL is devnull
        print output

    try:
        updateJsonDatalist = []
        updateJsonDatalist.append(json.loads(output)['data']['prepare_update'])
        updateJsonDatalist.append(json.loads(output)['data']['status'])
    except ValueError:
        print output
        return ''

    # print(updateJsonDatalist[jsonPrepareUpdate])
    # print(updateJsonDatalist[jsonStatus])

    return updateJsonDatalist

def getBIOSUpdateProgress():
    url = DestUrl+'/org/openbmc/control/flash/bios/action/GetBIOSUpdateProgress'
    cmds = ['curl', '-s', '-b', 'cjar', '-k', '-X', 'POST', '-H',
            'Content-Type: application/json', '-d',
            '{"data": []}', url]
    try:
        output = subprocess.check_output(cmds, stderr=FNULL)
    except CalledProcessError as e:
        # Sometimes curl fails with timeout error, let's ignore it
        return ''
    if FNULL is None:  # Do not print log when FNULL is devnull
        print output

    try:
        getJsonData = json.loads(output)['data']
    except ValueError:
        print output
        return ''

    return getJsonData

def controlChassisAction(method):

    url = DestUrl+'/redfish/v1/Systems/system/Actions/ComputerSystem.Reset'
    cmds = ['curl', '-s', '-b', 'cjar', '-k', '-X', 'POST', '-H',
            'Content-Type: application/json', '-d',
            '{"ResetType": "' + method + '"}', url]
    try:
        output = subprocess.check_output(cmds, stderr=FNULL)
    except CalledProcessError as e:
        # Sometimes curl fails with timeout error, let's ignore it
        return False

    if FNULL is None:  # Do not print log when FNULL is devnull
        print output

    # check_call(cmds, stdout=FNULL, stderr=FNULL)

    return True

def flashBIOSAction(method):
    url = DestUrl+'/org/openbmc/control/flash/bios/action/'+method
    cmds = ['curl', '-s', '-b', 'cjar', '-k', '-X', 'POST', '-H',
            'Content-Type: application/json', '-d',
            '{"data": []}', url]
    
    try:
        output = subprocess.check_output(cmds, stderr=FNULL)
    except CalledProcessError as e:
        # Sometimes curl fails with timeout error, let's ignore it
        return False

    if FNULL is None:  # Do not print log when FNULL is devnull
        print output

    # check_call(cmds, stdout=FNULL, stderr=FNULL)

    return True

def upload(bin):
    url = DestUrl+'/upload/bios'
    print 'Uploading \'%s\' to \'%s\' ...' % (bin, url)

    cmds = ['curl', '-k', '-H', 'Content-Type: application/octet-stream',
            '-X', 'POST', '-T', bin, url]
    # print cmds
    # check_call(cmds, stdout=FNULL, stderr=FNULL)

    try:
        output = subprocess.check_output(cmds, stderr=FNULL)
    except CalledProcessError as e:
        # Sometimes curl fails with timeout error, let's ignore it
        return ''
    if FNULL is None:  # Do not print log when FNULL is devnull
        print output
    
    try:
        getJsonData = json.loads(output)['data']
    except ValueError:
        print output
        return ''

    return getJsonData

def updateProcessing(status):
    processing = status.split('\n')
    for subString in processing:
        print '%s' %(subString)
    for clear in processing:
        deleteLastLine()

def update(bin):
    print 'Update...'

    print 'Upload via REST...'
    imagehash = upload(bin)

    flashBIOSAction('PrepareForUpdateBIOS')

    status = getUpdateJsonData()

    print(status[jsonStatus])

    if(status[jsonStatus] == prepareProcessing):
        while 1:
            getBIOSUpdateProgress()
            status = getUpdateJsonData()
            deleteLastLine()
            print(status[jsonStatus])
            if(status[jsonStatus] == prepareFinish):
                break
            sleep(1)

    timer = 0
    sleep(15)
    while 1 :
        status = getUpdateJsonData()

        if(status[jsonStatus] == prepareFinish):
            flashBIOSAction('SetMERecoveryMode')
            status = getUpdateJsonData()
            print(status[jsonStatus])
            sleep(5)
            flashBIOSAction('SwitchSPIInterface')
            status = getUpdateJsonData()
            print(status[jsonStatus])
            sleep(5)

        if(status[jsonStatus] == switchSPIFinish):
            flashBIOSAction('Apply')
            status = getUpdateJsonData()
            print(status[jsonStatus])
            deleteLastLine()
            sleep(5)

        if(status[jsonStatus] == writingFlash):
            while 1 : 
                processing = getBIOSUpdateProgress()
                subProcessing = processing.split('\n')
                updateProcessing(processing)
                if(subProcessing[0] == finishFlash):
                    print 'Flash image to bios via bmc finished...'
                    break
                if(subProcessing[0] == failedFlash):
                    print 'Flash image to bios via bmc failed...'
                    break
                if(timer > updateTimeout):
                    print 'Fail.Update timer timeout'
                    break
                sleep(1)
                +(+timer)

        if(status[jsonStatus] == finishFlash or status[jsonStatus] == failedFlash):
            flashBIOSAction('RestoreSPIInterface')
            status = getUpdateJsonData()
            print(status[jsonStatus])
            sleep(5)
            flashBIOSAction('SetMEReset')
            status = getUpdateJsonData()
            print(status[jsonStatus])
            sleep(5)
            controlChassisAction('On')
            print('Starting to power on server')
            break
        if(timer > updateTimeout):
            print 'Fail.Update timer timeout'
            break

        sleep(1)
        +(+timer)

    pass

def main():
    parser = argparse.ArgumentParser(
        description='[v%s] Upload bios bin file by REST api and update it to bios flash' %(utilityVersion))
    parser.add_argument('-f', '--file', required=True, dest='bin',
                        help='The bin file to upload and update')
    parser.add_argument('-b', '--bmc', required=True, dest='bmc',
                        help='The BMC IP address')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Print verbose log')
    parser.add_argument('-u', '--user', help='User name',dest='usr', default='root')
    parser.add_argument('-p', '--password', help='User password',dest='pw', default='root')

    args = parser.parse_args()

    args = vars(args)
    if args['bin'] is None or args['bmc'] is None:
        parser.print_help()
        sys.exit(1)
    global DestUrl
    DestUrl = 'https://'+args['usr']+':'+args['pw']+'@'+args['bmc']
    print (DestUrl)
    global FNULL
    if args['verbose']:
        FNULL = None  # Print log to stdout/stderr, for debug purpose
    else:
        FNULL = open(os.devnull, 'w')  # Redirect stdout/stderr to devnull

    if checkBmcAlive():
        print 'BMC is alive'
    else:
        print 'BMC is down, check it first'
        sys.exit(1)

    with tarfile.open('%s.tar.gz' %(args['bin']), 'w:gz') as uploadtarfile:
        uploadtarfile.add(args['bin'])

    update('%s.tar.gz' %(args['bin']))

    try:
        os.remove('%s.tar.gz' %(args['bin']))
    except OSError as e:
        print(e)

    print 'Completed!'

if __name__ == '__main__':
    main()
