#!/usr/bin/env python
"""
Usage: bmc_fw_update.py <--file tarball>
                            --bmc <bmc-ip>
                            [-v]

This scripts copies OpenBMC tarball to BMC,
and uses REST APIs to update the tarball to BMC.
"""

import argparse
import json
import os
import subprocess
from subprocess import check_output,CalledProcessError

from time import sleep

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

def activeImage(imagehash):
    url = DestUrl+'/xyz/openbmc_project/software/'+imagehash+'/attr/RequestedActivation'
    cmds = ['curl', '-s', '-k', '-H', 'Content-Type: application/json',
            '-X', 'PUT', '-d', '{"data": "xyz.openbmc_project.Software.Activation.RequestedActivations.Active"}', url]
    print cmds
    output = subprocess.check_output(cmds, stderr=FNULL)
    #check_call(cmds, stdout=FNULL, stderr=FNULL)

def reboot():
    url = DestUrl+"/org/openbmc/control/bmc0/action/warmReset"
    cmds = ['curl', '-s', '-b', 'cjar', '-k', '-X', 'POST', '-H',
            'Content-Type: application/json', '-d',
            '{"data": []}', url]
    output = subprocess.check_output(cmds, stderr=FNULL)        
    #check_call(cmds, stdout=FNULL, stderr=FNULL)

def computeHash(tarball):
    cmd = "tar xfO %s MANIFEST | sed -ne '/version=/ {s/version=//;p}' | head -n1 | tr -d '\n' | sha512sum | cut -b 1-8" % tarball
    try:
        output = subprocess.check_output(cmd, stderr=FNULL, shell=True)
    except CalledProcessError as e:
        # Sometimes curl fails with timeout error, let's ignore it
        return ''
    if FNULL is None:  # Do not print log when FNULL is devnull
        print output
    return output
    
def getActiveImageHash():
    url = DestUrl+'/xyz/openbmc_project/software/active'
    cmds = ['curl', '-k', url]
    try:
        output = subprocess.check_output(cmds, stderr=FNULL)
    except CalledProcessError as e:
        # Sometimes curl fails with timeout error, let's ignore it
        return ''
    if FNULL is None:  # Do not print log when FNULL is devnull
        print output
    activeimage = json.loads(output)['data']['endpoints'][0]
    activeimagehash = activeimage[-8:]
    return activeimagehash
        
    
def upload(tarball):
    url = DestUrl+'/upload/image'
    print 'Uploading \'%s\' to \'%s\' ...' % (tarball, url)

    cmds = ['curl', '-k', '-H', 'Content-Type: application/octet-stream',
            '-X', 'POST', '-T', tarball, url]
    print cmds
    #check_call(cmds, stdout=FNULL, stderr=FNULL)
    
    try:
        output = subprocess.check_output(cmds, stderr=FNULL)
    except CalledProcessError as e:
        # Sometimes curl fails with timeout error, let's ignore it
        return ''
    if FNULL is None:  # Do not print log when FNULL is devnull
        print output
        
    return json.loads(output)['data']


def update(tarball):
    #uploadimagehash = computeHash(tarball).strip()
    #activeimagehash = getActiveImageHash().encode().strip()
    #print 'activehash is %s, uploadhash is %s' % (activeimagehash, uploadimagehash)
    #if uploadimagehash == activeimagehash:
    #    print "The active image is the same as the image you want to upload, so skip the update."
    #    exit(1)
    #else:
    #    print 'Update...'

    print 'Upload via REST...'
    imagehash = upload(tarball)
    print imagehash

    activeImage(imagehash)

    print 'Reboot BMC...'
    reboot()
    sleep(120)
    while not checkBmcAlive():
        sleep(5)
    pass

def main():
    parser = argparse.ArgumentParser(
        description='[v0.05] upload tarball by REST and update it on BMC')
    parser.add_argument('-f', '--file', required=True, dest='tarball',
                        help='The tarball to upload and update')
    parser.add_argument('-b', '--bmc', required=True, dest='bmc',
                        help='The BMC IP address')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Print verbose log')
    parser.add_argument("-u", "--user", help="user name",dest="usr", default="root")
    parser.add_argument("-p", "--password", help="password",dest="pw", default="0penBmc")

    args = parser.parse_args()
    #print(args.usr)
    args = vars(args)
    if args['tarball'] is None or args['bmc'] is None:
        parser.print_help()
        exit(1)
    global DestUrl
    DestUrl = "https://"+args["usr"]+":"+args["pw"]+"@"+args["bmc"]
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
        exit(1)

    update(args['tarball'])

    print 'Completed!'

if __name__ == "__main__":
    main()
