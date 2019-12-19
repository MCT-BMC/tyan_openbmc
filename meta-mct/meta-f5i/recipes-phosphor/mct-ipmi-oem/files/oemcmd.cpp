#include "oemcmd.hpp"
#include <host-ipmid/ipmid-api.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <systemd/sd-bus.h>
#include <endian.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include <linux/types.h>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/bus/match.hpp>
#include <sdbusplus/message.hpp>
#include <sdbusplus/vtable.hpp>
#include <sdbusplus/server/interface.hpp>
#include <sdbusplus/timer.hpp>
#include <gpiod.hpp>

void register_netfn_mct_oem() __attribute__((constructor));

std::unique_ptr<phosphor::Timer> clrCmosTimer = nullptr;
#define AST_GPIO_F1_PIN 41 
bool clrCmos()
{
#if 0    
    //todo : move to device tree by assigned name 
    auto line = gpiod::find_line("CLR_CMOS");
        
    if (!line)
    {
        return false;
    }
#endif 
    auto chip = gpiod::chip("gpiochip0");
    auto line = chip.get_line(AST_GPIO_F1_PIN);
    if (!line)
    {
        std::cerr << "Error requesting gpio AST_GPIO_F1_PIN\n";
        return false;
    }
#if 0
    auto dir = line.direction();
    std::cerr << "direction :" <<dir<<"\n";
#endif 
    int value = 1;

    try
    {
        line.request({"ipmid", gpiod::line_request::DIRECTION_OUTPUT,0}, value);
    }
    catch (std::system_error&)
    {
        std::cerr << "Error requesting gpio\n";
        return false;
    }

    line.set_value(0);
    sleep(3);
    line.set_value(1);
    
    line.release();
    return true;

}
void createTimer()
{
    if (clrCmosTimer == nullptr)
    {
        clrCmosTimer = std::make_unique<phosphor::Timer>(clrCmos);
    }
}

/*
    NetFun: 0x3e
    Cmd : 0x3a
    Request:
*/
ipmi_ret_t ipmiOpmaClearCmos(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                              ipmi_request_t request, ipmi_response_t response,
                              ipmi_data_len_t data_len, ipmi_context_t context)
{
    ipmi_ret_t ipmi_rc = IPMI_CC_OK;

    //todo, check pgood status
    createTimer();
    if (clrCmosTimer == nullptr)
    {
        return IPMI_CC_RESPONSE_ERROR;
    }
    
    if(clrCmosTimer->isRunning())
    {
        return IPMI_CC_RESPONSE_ERROR;
    }
    clrCmosTimer->start(std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::seconds(0)));    
   
    return ipmi_rc;

}

void register_netfn_mct_oem()
{
    ipmi_register_callback(NETFUN_TWITTER_OEM, IPMI_CMD_ClearCmos, NULL, ipmiOpmaClearCmos, PRIVILEGE_ADMIN);
}
