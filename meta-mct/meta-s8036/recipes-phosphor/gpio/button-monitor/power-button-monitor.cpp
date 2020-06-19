#include "power-button-monitor.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <string>

#define _POSIX_C_SOURCE 200809L
#define POLL_TIMEOUT 1
#define MAX_BUFFER 64

#define DEBUG 0

int main(int argc, char *argv[])
{
    struct pollfd fdset[2];
    char *buf[MAX_BUFFER];
    int gpioFd, timeout, rc;
    int nfds = 2;
    bool initWorkaround = true;

    Button button;
    button.gpioBase = findGpioBase();
    button.gpioE0 = button.gpioBase + 32;
    button.gpioF2 = button.gpioBase + 42;

    if(DEBUG){
        std::cerr << "Power Button monitor start\n";
    }

    while(1){
        gpioFd = gpioFdOpen(button.gpioE0);

        if(gpioFd < 0){
            setGpioExport(button.gpioE0);
            usleep(500000);
            continue;
        }

        timeout = POLL_TIMEOUT;

        while (1) {
            memset((void*)fdset, 0, sizeof(fdset));

            fdset[0].fd = STDIN_FILENO;
            fdset[0].events = POLLIN;
            fdset[1].fd = gpioFd;
            fdset[1].events = POLLPRI;

            rc = poll(fdset, nfds, timeout);

            if (rc < 0) {
                if(DEBUG){
                    std::cerr << "Power Button Poll failed\n";
                }
                break;
            }

            if(button.buttonStatus && buttonTimeout(button.wait,POLL_TIMEOUT)){
                //Reset waiting timer
                button.wait = getCurrentTimeWithMs();
                buttonWaiting(&button);
            }

            if(fdset[1].revents & POLLPRI) {
                lseek(fdset[1].fd, 0, SEEK_SET);
                read(fdset[1].fd, buf, MAX_BUFFER);

                // To avoid first times input file's value
                if(initWorkaround){
                    initWorkaround=false;
                    continue;
                }

                button.buttonValue = ((unsigned int) buf[0])%2;

                if(button.buttonValue){
                    buttonRelease(&button);
                }
                else{
                    buttonPress(&button);
                }
            }

            fflush(stdout);
            usleep(50000);
        }

        close(gpioFd);
    }

    return 0;
}

void buttonPress(Button *button)
{
    if(DEBUG){
        std::cerr << "Power Button Press\n";
    }

    if(!button->buttonStatus){
        button->pgood = getPowerGoodValue();
        button->start = getCurrentTimeWithMs();
        button->wait = getCurrentTimeWithMs();

        if(!button->pgood){
            // To set power button press and release by software
            system("/usr/bin/gpioset `gpiofind PWBTN_OUT`=0");
            usleep(100000);
            system("/usr/bin/gpioset `gpiofind PWBTN_OUT`=1");
        }

        button->buttonStatus = true;
    }
}

void buttonRelease(Button *button)
{
    if(DEBUG){
        std::cerr << "Power Button Release\n";
    }

    if(button->buttonStatus){
        button->finish = getCurrentTimeWithMs();
        button->buttonStatus = false;
        button->unlockEnable = false;
        button->counter = 0;
        // To lock button
        setGpioValue(0,button->gpioF2);
    }
}

void buttonWaiting(Button *button)
{
    if(DEBUG){
        std::cerr << "Power Button poll waiting...\n";
    }

    if(button->buttonStatus){
        // keep counting when button pressed
        button->counter = button->counter +1;
    }
    if(((button->counter >=2 && button->pgood) || button->counter >= 2) && !button->unlockEnable){
        // "button->counter >=2 && !button->pgood" using when host power-on

        if(DEBUG){
            button->finish = getCurrentTimeWithMs();
            button->duration = (double)(button->finish - button->start);
            std::cerr << "To unlock button "<< button->duration <<" \n";
        }
        // To unlock button
        setGpioValue(1,button->gpioF2);

        button->unlockEnable = true;
    }
}