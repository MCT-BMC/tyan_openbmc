#!/usr/bin/env python
"""
Usage: cpld_fw_update.py 
                                --file <bin-file> 
                                --bmc <bmc-ip> 
                                [-v]

This scripts copies cpld bin file to BMC,
and uses REST APIs to update the bin file to CPLD via bmc.
"""

import argparse
import json
import os
import sys
import subprocess
import tarfile
from subprocess import check_output,CalledProcessError
from time import sleep

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

    f = open('MANIFEST', 'w')
    f.write('purpose=%s\n' %(purposeWrite))
    f.write('version=%s\n' %(versionWrite))
    f.write('MachineName=%s\n' %(machineWrite))
    f.close()

    with tarfile.open('%s.tar.gz' %(bin), 'w:gz') as uploadtarfile:
        uploadtarfile.add(bin)
        uploadtarfile.add('MANIFEST')


def update(bin):
    print 'Update...'

    print 'Upload via REST...'
    versionId = upload(bin)

    activeImage(versionId)

    powerOffFlag=1
    powerOffWaitFlag=0
    programFlag=0
    verificationFlag=0
    timer=0
    dot=''

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

def main():
    parser = argparse.ArgumentParser(
        description='[v%s] Upload cpld bin file by REST api and update it to cpld flash' %(utilityVersion))
    parser.add_argument('-f', '--file', required=True, dest='bin',
                        help='The bin file to upload and update')
    parser.add_argument('-b', '--bmc', required=True, dest='bmc',
                        help='The BMC IP address')
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

    genUploadFile(args['bin'],args['machine'],args['version'],'cpld')
    update('%s.tar.gz' %(args['bin']))

    try:
        os.remove('%s.tar.gz' %(args['bin']))
    except OSError as e:
        print(e)

    print 'Completed!'

if __name__ == '__main__':
    main()
