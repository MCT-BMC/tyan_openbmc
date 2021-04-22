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

void setGpioToBmc()
{
    setOutput("BIOS_SPI_SW",1);
}

void setGpioToBios()
{
    setOutput("BIOS_SPI_SW",0);
}

int biosUpdate(char* image_str)
{
    std::string flashcp_str = "/usr/sbin/flashcp -v ";
    flashcp_str = flashcp_str + image_str + " /dev/mtd/pnor";

    system(flashcp_str.c_str());

    return 0;
}

int main(int argc, char *argv[])
{
    auto current = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(current);

    std::cerr << "Bios upgrade started at " << std::ctime(&currentTime);

    if (argc < 2)
    {
        std::cerr << "Usage: bios-update [file name] : update bios firmware" << std::endl;
        return 0;
    }

    if (access(argv[1], F_OK) < 0)
    {
        std::cerr << "Bios image " << argv[1] << " doesn't exist" << std::endl;
        return -1;
    }
    else
    {
        std::cerr << "Bios image is " << argv[1] << std::endl;
    }


    std::cerr << "Power off host server" << std::endl;
    initiateStateTransition("Off");
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
    setMeToRecoveryMode();
    sleep(5);

    std::cerr << "Set GPIO to access SPI flash from BMC used by host" << std::endl;
    setGpioToBmc();
    sleep(5);

    std::cerr << "Bind aspeed-smc spi driver" << std::endl;
    setBiosMtdDevice(BIND);
    sleep(1);

    std::cerr << "Store reserved mtd address data" << std::endl;
    storeReservedAddress();
    sleep(1);
    
    std::cerr << "Flashing bios image..." << std::endl;
    biosUpdate(argv[1]);
    std::cerr << "bios updated successfully..." << std::endl;
    sleep(1);

    std::cerr << "Recovery reserved mtd address data" << std::endl;
    recoveryReservedAddress();
    sleep(1);

    std::cerr << "Unbind aspeed-smc spi driver" << std::endl;
    setBiosMtdDevice(UNBIND);
    sleep(10);

    std::cerr << "Set GPIO back for host to access SPI flash" << std::endl;
    setGpioToBios();
    sleep(5);

    std::cerr << "Reset ME to boot from new bios" << std::endl;
    setMeReset();
    sleep(10);

    std::cerr << "Power on server" << std::endl;
    initiateStateTransition("On");
    sleep(10);

    if(!checkHostPower())
    {
        std::cerr << "Powering on server again" << std::endl;
        initiateStateTransition("On");
    }

    std::cerr << "BIOS updated successfully..." << std::endl;
    return 0;
}