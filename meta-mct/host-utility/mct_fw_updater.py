#!/usr/bin/env python
"""
Usage: mct_fw_update.py 
                                --file <bin-file> 
                                --bmc <bmc-ip> 
                                --device <updating-device (bmc , bios , cpld)>
                                [-v]

This scripts copies firmware image file to BMC,
and uses REST APIs to update the firmware image file to purpose device via bmc.
"""

import argparse
import json
import os
import sys
import subprocess
import tarfile
from subprocess import check_output,CalledProcessError
from time import sleep
from os import path

utilityVersion = '0.01'

updateTimeout = 600
jsonPrepareUpdate = 0

def deleteLastLine():
    # cursor up one line
    sys.stdout.write('\x1b[1A')
    # delete last line
    sys.stdout.write('\x1b[2K')


def checkBmcAlive():
    url = DestUrl+'/xyz/openbmc_project/'
    cmds = ['curl', '-k', url]
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

def setBmcReboot():
    url = DestUrl+"/org/openbmc/control/bmc0/action/warmReset"
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
    #check_call(cmds, stdout=FNULL, stderr=FNULL)

def activeImage(versionId):
    url = DestUrl+'/xyz/openbmc_project/software/' + versionId + '/attr/RequestedActivation'
    cmds = ['curl', '-s', '-k', '-H', 'Content-Type: application/json',
            '-X', 'PUT', '-d', '{"data": "xyz.openbmc_project.Software.Activation.RequestedActivations.Active"}', url]
    try:
        output = subprocess.check_output(cmds, stderr=FNULL)
    except CalledProcessError as e:
        # Sometimes curl fails with timeout error, let's ignore it
        return False

    if FNULL is None:  # Do not print log when FNULL is devnull
        print output
    #check_call(cmds, stdout=FNULL, stderr=FNULL)

def getUpdateProgress(versionId):
    url = DestUrl+'/xyz/openbmc_project/software/' + versionId + '/attr/Progress'
    cmds = ['curl', '-s', '-b', 'cjar', '-k', '-X', 'GET', '-H',
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
        getJsonStatus = json.loads(output)['status']
    except ValueError:
        print output
        return ''

    if getJsonStatus == 'error':
        return '-1'

    return getJsonData

def upload(bin):
    url = DestUrl+'/upload/image'
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

def genUploadFile(bin,machine,version,purpose):
    # Remove previous file to be safe
    try:
        os.remove('MANIFEST')
    except OSError as e:
        print(e)
    machineWrite=''
    versionWrite='%s' %(bin)
    purposeWrite=''

    if machine!='':
        machineWrite = machine

    if version!='':
        versionWrite = version

    if purpose=='cpld':
        purposeWrite='xyz.openbmc_project.Software.Version.VersionPurpose.CPLD'
    if purpose=='bios':
        purposeWrite='xyz.openbmc_project.Software.Version.VersionPurpose.Host'

    f = open('MANIFEST', 'w')
    f.write('purpose=%s\n' %(purposeWrite))
    f.write('version=%s\n' %(versionWrite))
    f.write('MachineName=%s\n' %(machineWrite))
    f.close()

    with tarfile.open('%s.tar.gz' %(bin), 'w:gz') as uploadtarfile:
        uploadtarfile.add(bin)
        uploadtarfile.add('MANIFEST')


def update(file,purpose):
    print 'Update...'

    print 'Upload via REST...'
    versionId = upload(file)

    activeImage(versionId)

    powerOffFlag=1
    powerOffWaitFlag=0
    prepareFlag=0
    programFlag=0
    bmcRebootFlag=0
    verificationFlag=0
    initialFlag=0
    recoveryFlag=0
    displayFlag=1
    timer=0
    dot=''

    if purpose=='cpld':
        while 1 :
            progress=getUpdateProgress(versionId)
            if powerOffFlag and int(progress) >= 10 :
                print 'Starting system power off...'
                print 'Waiting for system power off...'
                powerOffFlag = 0
                powerOffWaitFlag=1
            if powerOffWaitFlag and int(progress) >= 30 :
                print 'Starting to erase and program image...'
                powerOffWaitFlag = 0
                programFlag=1
            if programFlag and int(progress) >= 80 :
                print 'Starting to verify image...'
                programFlag=0
                verificationFlag=1
            if int(progress) == -1 :
                if verificationFlag :
                    print 'Updating progress 100%... finished'
                else :
                    print 'Updating progress failed'
                break

            if timer >= updateTimeout:
                print 'Updating progress failed'
                break

            if timer%2 :
                dot = '.'
            else :
                dot = ''
            print 'Updating progress %s%%....%s' %(progress, dot)
            deleteLastLine()
            sleep(1)
            timer = timer + 1
        pass
    if purpose=='bios':
        while 1 :
            progress=getUpdateProgress(versionId)
            if powerOffFlag and int(progress) >= 5 :
                print 'Starting system power off...'
                print 'Waiting for system power off...'
                powerOffFlag = 0
                powerOffWaitFlag=1
            if powerOffWaitFlag and int(progress) >= 10 :
                print 'Starting to setting the updating mode...'
                powerOffWaitFlag = 0
                prepareFlag=1
            if prepareFlag and int(progress) >= 30 :
                print 'Starting to erase and program image...'
                prepareFlag = 0
                programFlag=1
            if programFlag and int(progress) >= 70 :
                print 'Starting to verify image...'
                programFlag=0
                verificationFlag=1
            if verificationFlag and int(progress) >= 90 :
                print 'Starting to setting the normal mode...'
                verificationFlag=0
                recoveryFlag=1
            if int(progress) == -1 :
                if recoveryFlag :
                    print 'Updating progress 100%... finished'
                else :
                    print 'Updating progress failed'
                break

            if timer >= updateTimeout:
                print 'Updating progress failed'
                break

            if timer%2 :
                dot = '.'
            else :
                dot = ''
            print 'Updating progress %s%%....%s' %(progress, dot)
            deleteLastLine()
            sleep(1)
            timer = timer + 1
        pass
    if purpose=='bmc':
        progress=20
        programFlag=1
        bmcRebootFlag=1
        while 1 :
            if programFlag and int(progress) >= 20 :
                if displayFlag :
                    print 'Starting to erase,program and verify image...'
                    displayFlag=0
                if bmcRebootFlag :
                    setBmcReboot()
                    bmcRebootFlag=0
                progress=20 + int(timer/2)
                if(progress  >= 90) :
                    programFlag=0
                    initialFlag=1
                    displayFlag=1
            if initialFlag and int(progress) >= 90 :
                if displayFlag :
                    print 'Checking bmc initializing...'
                    print 'Updating progress %s%%....%s' %(progress, dot)
                    deleteLastLine()
                    displayFlag=0
                if(progress  <= 95) :
                    progress=20 + int(timer/2)
                if checkBmcAlive():
                    initialFlag = 0
                    recoveryFlag=1
            if int(progress) != -1 and recoveryFlag :
                if recoveryFlag :
                    print 'Updating progress 100%... finished'
                else :
                    print 'Updating progress failed'
                break

            if timer >= updateTimeout:
                print 'Updating progress failed'
                break

            if timer%2 :
                dot = '.'
            else :
                dot = ''
            print 'Updating progress %s%%....%s' %(progress, dot)
            deleteLastLine()
            sleep(1)
            timer = timer + 1
        pass

def main():
    parser = argparse.ArgumentParser(
        description='[v%s] Upload firmware image file by REST api and update it to purpose device' %(utilityVersion))
    parser.add_argument('-f', '--file', required=True, dest='file',
                        help='The firmware image file to upload and update')
    parser.add_argument('-b', '--bmc', required=True, dest='bmc',
                        help='The BMC IP address')
    parser.add_argument('-d', '--device', required=True, dest='device',
                        help='The updating device ( bmc, bios, cpld )')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Print verbose log')
    parser.add_argument('-u', '--user', help='User name',dest='usr', default='root')
    parser.add_argument('-p', '--password', help='User password',dest='pw', default='root')
    parser.add_argument('-m', '--machine', help='Optionally specify the target machine name of this image.',
                        dest='machine', default='')
    parser.add_argument('-V', '--Version', help='Specify the version of image file',
                        dest='version', default='')

    args = parser.parse_args()

    args = vars(args)
    if args['file'] is None or args['bmc'] is None or  args['device'] is None :
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

    if path.exists(args['file'])==False :
        print 'Please check that the file exists'
        sys.exit(1)

    if args['device']!='bios' and args['device']!='cpld' and args['device']!='bmc' :
        print 'Invalid device for updating purpose'
        sys.exit(1)
    
    if args['device']=='bmc' and args['file'].find("tar")==-1 :
        print 'Invalid format for ipnut file. Please input the file with tar foramt'
        sys.exit(1)

    if args['device']=='cpld' and args['file'].find("jed")==-1 :
        print 'Invalid format for ipnut file. Please input the file with jed foramt'
        sys.exit(1)

    if checkBmcAlive():
        print 'BMC is alive'
    else:
        print 'BMC is down, check it first'
        sys.exit(1)

    if args['device']=='bios' or args['device']=='cpld' :
        genUploadFile(args['file'],args['machine'],args['version'],args['device'])
        update('%s.tar.gz' %(args['file']),args['device'])
    if args['device']=='bmc' : 
        update(args['file'],args['device'])

    try:
        if args['device']=='bios' or args['device']=='cpld' :
            os.remove('%s.tar.gz' %(args['file']))
    except OSError as e:
        print(e)

    print 'Completed!'

if __name__ == '__main__':
    main()
