#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>

#define MAX_BUFFER 64 //Maximum size for char array

/*
 * @desc Find gpio base number in aspeed kernel's gpio path
 * @param null
 * @return success: gpio base number fail: zero
 */
unsigned int findGpioBase()
{
    unsigned int result = 0;
    char buf[MAX_BUFFER];
    FILE *fp;

    if ((fp = popen("ls /sys/bus/platform/devices/1e780000.gpio/gpio |  grep -i gpiochip | cut -c 9-", "r")) == NULL){
        return result;
    }

    fgets(buf, sizeof(buf), fp);
    pclose(fp);

    sscanf(buf, "%d", &result);

    return result;
}

/*
 * @desc Export gpio using sysfs path
 * @param gpio - gpio number
 * @return true: export success false: export fail
 */
bool setGpioExport(unsigned int gpio)
{
    unsigned int result = 0;
    FILE * fp;
    char buf[MAX_BUFFER];

    sprintf(buf, "echo %d > /sys/class/gpio/export",gpio);

    if ((fp = popen(buf, "r")) == NULL){
        return false;
    }

    pclose(fp);

    return true;
}

/*
 * @desc Get gpio current value using sysfs path
 * @param gpio - gpio number
 * @return success: gpio current value fail: zero
 */
unsigned int getGpioValue(unsigned int gpio)
{
    unsigned int result = 0;
    FILE * fp;
    char buf[MAX_BUFFER];
    char valuePath[MAX_BUFFER];

    sprintf(valuePath, "cat /sys/class/gpio/gpio%d/value",gpio);

    if ((fp = popen(valuePath, "r")) == NULL){
        return result;
    }

    fgets(buf, sizeof(buf), fp);
    pclose(fp);

    sscanf(buf, "%d", &result);

    return result;
}

/*
 * @desc Set gpio current value using sysfs path
 * @param gpio - gpio number
 * @return true: get success false: get fail
 */
bool setGpioValue(unsigned int set,unsigned int gpio)
{
    unsigned int result = 0;
    FILE * fp;
    char unlockPath[MAX_BUFFER];

    sprintf(unlockPath, "echo %d > /sys/class/gpio/gpio%d/value",set,gpio);

    if ((fp = popen(unlockPath, "r")) == NULL){
        return false;
    }

    pclose(fp);

    return true;
}

/*
 * @desc Get power good current value in dbus using bus control
 * @param null
 * @return success: power good current value fail: zero
 */
unsigned int getPowerGoodValue()
{
    unsigned int result = 0;
    char buf[MAX_BUFFER];
    FILE *fp;

    if ((fp = popen("busctl get-property org.openbmc.control.Power /org/openbmc/control/power0 org.openbmc.control.Power pgood | cut -d' ' -f2", "r")) == NULL){
        return result;
    }

    fgets(buf, sizeof(buf), fp);
    pclose(fp);

    sscanf(buf, "%d", &result);

    return result;
}

/*
 * @desc Get current timestamp in milliseconds
 * @param null
 * @return current timestamp in milliseconds
 */
double getCurrentTimeWithMs()
{
    time_t s;
    long ms;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    if (ms > 999) {
        s++;
        ms = 0;
    }

    double  result = (intmax_t)s + ((double)ms/1000);

    return result;
}

/*
 * @desc Using open function for gpio value path in sysfs
 * @param gpio - gpio number
 * @return open's response code
 */
int gpioFdOpen(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUFFER];

    len = snprintf(buf, sizeof(buf),"/sys/class/gpio/gpio%d/value", gpio);

    fd = open(buf, O_RDONLY | O_NONBLOCK );

    return fd;
}

/*
 * @desc Check timerout for a period
 * @param start - input time in period
 * @param timeout - timeout time for a period
 * @return true: timeout false: in period
 */
bool buttonTimeout(double start,int timeout)
{
    double current = getCurrentTimeWithMs();
    double duration = (double)(current - start);

    if(duration > (double)timeout){
        return true;
    }
    return false;
}