/* Copyright 2020 MCT
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "bios-update.hpp"
#include "reserved-address.hpp"
#include <getopt.h>

typedef struct {
    int program;                    /* enable/disable program  */
    int debug;                      /* enable/disable debug flag */
    int progress;                   /* progress update, use the version id */
    char version_id[256];         /* version id */
}bios_t;

static void usage(FILE *fp, int argc, char **argv)
{
    fprintf(fp,
            "Usage: %s [options]\n\n"
            "Options:\n"
            " -h | --help                   Print this message\n"
            " -p | --program                Program bios and verify\n"
            " -d | --debug                  debug mode\n"
            " -P | --Progress               Progress update with version id\n"
            "",
            argv[0]);
}

static const char short_options [] = "dhp:P:";

static const struct option
    long_options [] = {
    { "help",       no_argument,        NULL,    'h' },
    { "program",    required_argument,  NULL,    'p' },
    { "debug",      no_argument,        NULL,    'd' },
    { "Progress",   required_argument,  NULL,    'P' },
    { 0, 0, 0, 0 }
};

static void update_flash_progress(int pecent, const char *version_id, int progress)
{
    if(progress){
        FILE * fp;
        char buf[100];
        sprintf(buf, "flash-progress-update %s %d", version_id, pecent);
        if ((fp = popen(buf, "r")) == NULL){
            return ;
        }
        pclose(fp);
    }
    return;
}

void setUpdateProgress(bios_t* bios, char* input, uint8_t base, uint8_t divisor)
{
    std::string buffer(input);
    int start = buffer.find("(", 0);
    int end = buffer.find("%", 0);
    int progress = base + (atoi(buffer.substr(start+1,end-start-1).c_str())/ divisor);
    update_flash_progress(progress,bios->version_id,bios->progress);
}

int biosUpdate(bios_t* bios, char* image_str)
{
    FILE *fp;
    char buffer[100];

    std::string flashcp_str = "/usr/sbin/flashcp -v ";
    std::string erase = "Erasing";
    std::string write = "Writing";
    std::string verify = "Verifying";

    uint8_t base = 30;
    uint8_t divisor = 5;
    uint8_t delay = 16;
    uint8_t counter = 0;

    flashcp_str = flashcp_str + image_str + " /dev/mtd/pnor";

    fp = popen(flashcp_str.c_str(), "r");

    if (fp == NULL) {
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if(counter==0){
            counter = delay;
            printf("%s\n",buffer);
            if(bios->progress){
                if(strstr(buffer, erase.c_str()) != NULL)
                {
                    setUpdateProgress(bios,buffer,base,divisor);
                }
                else if(strstr(buffer, write.c_str()) != NULL)
                {
                    setUpdateProgress(bios,buffer,base+(100/divisor),divisor);
                }
                else if(strstr(buffer, verify.c_str()) != NULL)
                {
                    setUpdateProgress(bios,buffer,base+((100/divisor)*2),divisor);
                }
            }
            // printf("\033[1A");
            // printf("\033[K");
        }
        counter--;
    }

    pclose(fp);

    return 0;
}

void setGpioToBmc()
{
    setOutput("BIOS_SPI_SW",1);
}

void setGpioToBios()
{
    setOutput("BIOS_SPI_SW",0);
}

int main(int argc, char *argv[])
{
    char option;
    bios_t bios;
    char in_name[100] = "";

    if(argc <= 2) {
        usage(stdout, argc, argv);
        exit(EXIT_SUCCESS);
    }

    memset(&bios, 0, sizeof(bios));
    bios.program = 0; /* Setting default program progress*/
    bios.progress = 0; /* Setting default progress disable*/

    while ((option = getopt_long(argc, argv, short_options, long_options, NULL)) != (char) -1) {
        switch (option) {
        case 'h':
            usage(stdout, argc, argv);
            exit(EXIT_SUCCESS);
            break;
        case 'p':
            bios.program = 1;
            strcpy(in_name, optarg);
            if (!strcmp(in_name, "")) {
                printf("No input file name!\n");
                usage(stdout, argc, argv);
                exit(EXIT_FAILURE);
            }
            break;

        case 'd':
            bios.debug = 1;
            break;

        case 'P':
            bios.progress = 1;
            strcpy(bios.version_id, optarg);
            if (!strcmp(bios.version_id, "")) {
                printf("No input version id!\n");
                usage(stdout, argc, argv);
                exit(EXIT_FAILURE);
            }
            break;
        default:
            usage(stdout, argc, argv);
            exit(EXIT_FAILURE);
        }
    }

    if(!bios.program)
    {
        printf("No updating progress requested\n");
        exit(EXIT_SUCCESS);
    }

    auto current = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(current);

    std::cerr << "Bios upgrade started at " << std::ctime(&currentTime);

    if (access(in_name, F_OK) < 0)
    {
        std::cerr << "Bios image " << in_name << " doesn't exist" << std::endl;
        return -1;
    }
    else
    {
        std::cerr << "Bios image is " << in_name << std::endl;
    }


    std::cerr << "Power off host server" << std::endl;
    update_flash_progress(5,bios.version_id,bios.progress);
    initiateStateTransition("Off");
    update_flash_progress(10,bios.version_id,bios.progress);
    sleep(15);

    if(checkHostPower())
    {
        std::cerr << "Host server didn't power off" << std::endl;
        std::cerr << "Bios upgrade failed" << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "Host server powered off" << std::endl;
    }

    std::cerr << "Set ME to recovery mode" << std::endl;
    update_flash_progress(15,bios.version_id,bios.progress);
    setMeToRecoveryMode();
    sleep(5);

    std::cerr << "Set GPIO to access SPI flash from BMC used by host" << std::endl;
    update_flash_progress(20,bios.version_id,bios.progress);
    setGpioToBmc();
    sleep(5);

    std::cerr << "Bind aspeed-smc spi driver" << std::endl;
    update_flash_progress(23,bios.version_id,bios.progress);
    setBiosMtdDevice(BIND);
    sleep(1);

    std::cerr << "Store reserved mtd address data" << std::endl;
    update_flash_progress(26,bios.version_id,bios.progress);
    storeReservedAddress();
    sleep(1);
    
    std::cerr << "Flashing bios image..." << std::endl;
    update_flash_progress(30,bios.version_id,bios.progress);
    biosUpdate(&bios,in_name);
    std::cerr << "bios updated successfully..." << std::endl;
    sleep(1);

    std::cerr << "Recovery reserved mtd address data" << std::endl;
    recoveryReservedAddress();
    sleep(1);

    std::cerr << "Unbind aspeed-smc spi driver" << std::endl;
    update_flash_progress(92,bios.version_id,bios.progress);
    setBiosMtdDevice(UNBIND);
    sleep(10);

    std::cerr << "Set GPIO back for host to access SPI flash" << std::endl;
    update_flash_progress(94,bios.version_id,bios.progress);
    setGpioToBios();
    sleep(5);

    std::cerr << "Reset ME to boot from new bios" << std::endl;
    update_flash_progress(96,bios.version_id,bios.progress);
    setMeReset();
    sleep(10);

    std::cerr << "Power on server" << std::endl;
    update_flash_progress(98,bios.version_id,bios.progress);
    initiateStateTransition("On");
    sleep(10);

    if(!checkHostPower())
    {
        std::cerr << "Powering on server again" << std::endl;
        update_flash_progress(99,bios.version_id,bios.progress);
        initiateStateTransition("On");
    }

    std::cerr << "BIOS updated successfully..." << std::endl;
    return 0;
}