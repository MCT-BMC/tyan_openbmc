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

#include <ipmid/api.hpp>
#include <ipmid/utils.hpp>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/message/types.hpp>

#define FSC_SERVICE "xyz.openbmc_project.EntityManager"
#define FSC_OBJECTPATH "/xyz/openbmc_project/inventory/system/board/s7106_Baseboard/Pid_"
#define PID_INTERFACE "xyz.openbmc_project.Configuration.Pid.Zone"
#define PROPERTY_INTERFACE "org.freedesktop.DBus.Properties"

namespace ipmi
{

static int getProperty(sdbusplus::bus::bus& bus, const std::string& path,
                 const std::string& property, double& value, const std::string service, const std::string interface)
{
    auto method = bus.new_method_call(service.c_str(), path.c_str(), PROPERTY_INTERFACE, "Get");
    method.append(interface.c_str(),property);
    auto reply=bus.call(method);
    if (reply.is_method_error())
    {
        std::printf("Error looking up services, PATH=%s",interface.c_str());
        return -1;
    }

    sdbusplus::message::variant<double> valuetmp;
    try
    {
        reply.read(valuetmp);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        std::printf("Failed to get pattern string for match process");
        return -1;
    }

    value = std::get<double>(valuetmp);
    return 0;
}

static void setProperty(sdbusplus::bus::bus& bus, const std::string& path,
                 const std::string& property, const double value)
{
    auto method = bus.new_method_call(FSC_SERVICE, path.c_str(),
                                      PROPERTY_INTERFACE, "Set");
    method.append(PID_INTERFACE, property, sdbusplus::message::variant<double>(value));
	
    bus.call_noreply(method);

    return;
}

void register_netfn_mct_oem() __attribute__((constructor));

int execmd(char* cmd,char* result) {
        char buffer[128];
        FILE* pipe = popen(cmd, "r");
        if (!pipe)
                return -1;

        while(!feof(pipe)) {
                if(fgets(buffer, 128, pipe)){
                        strcat(result,buffer);
                }
        }
        pclose(pipe);
        return 0;
}

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
//===============================================================
/* Set Fan Control Enable Command
NetFun: 0x2E
Cmd : 0x06
Request:
        Byte 1-3 : Tyan Manufactures ID (FD 19 00)
        Byte 4 :  0h - Fan Control Disable
                  1h - Fan Control Enable
                  ffh - Get Current Fan Control Status
Response:
        Byte 1 : Completion Code
        Byte 2-4 : Tyan Manufactures ID
        Byte (5) : Current Fan Control Status , present if FFh passed to Enable Fan Control in Request
*/
ipmi::RspType<uint8_t> ipmi_tyan_ManufactureMode(uint8_t mode)
{
    std::fstream file;
    int rc=0;
    char command[100];
    char FSCStatus[100];
    uint8_t currentStatus;

    if (mode == 0)
    {
        // Disable Fan Control
        memset(command,0,sizeof(command));
        sprintf(command, "systemctl stop phosphor-pid-control.service");
        rc = system(command);
    
        memset(command,0,sizeof(command));
        sprintf(command, "echo 0 > /usr/sbin/fsc");
        system(command);

		// Set all fan to Full Duty
		for(size_t i = 1; i <= 5; i++)
		{
			memset(command,0,sizeof(command));
			sprintf(command, "echo 255 > /sys/class/hwmon/hwmon0/pwm%d",i);
			rc = system(command);
		}
    }
    else if (mode == 1)
    {
        // Enable Fan Control
        memset(command,0,sizeof(command));
        sprintf(command, "systemctl start phosphor-pid-control.service");
        rc = system(command);

        memset(command,0,sizeof(command));
        sprintf(command, "echo 1 > /usr/sbin/fsc");
        system(command);
    }
    else if (mode == 0xff)
    {
        // Get Current Fan Control Status
        file.open("/usr/sbin/fsc",std::ios::in);
        if(!file)
        {
            memset(command,0,sizeof(command));
            sprintf(command, "touch /usr/sbin/fsc");
            system(command);

            memset(command,0,sizeof(command));
            sprintf(command, "echo 1 > /usr/sbin/fsc");
            system(command);

            currentStatus=1;
        }
        else
        {
            file.read(FSCStatus,sizeof(FSCStatus));
            currentStatus = strtol(FSCStatus,NULL,16);
        }
        file.close();
        return ipmi::responseSuccess(currentStatus);
    }
    else
    {
        return ipmi::responseUnspecifiedError();
    }

    if (rc != 0)
    {
        return ipmi::responseUnspecifiedError();
    }

    return ipmi::responseSuccess();
}
//===============================================================

/* Set Fan Control PWM Duty Command
NetFun: 0x2E
Cmd : 0x05
Request:
        Byte 1-3 : Tyan Manufactures ID (FD 19 00)
        Byte 4 : PWM ID ( 0h-PWM1 , ... , 4h-PWM5 )
        Byte 5 : Duty Cycle
            0-64h - manual control duty cycle (0%-100%)
            FEh - Get current duty cycle
            FFh - Return to automatic control
Response:
        Byte 1 : Completion Code
        Byte 2-4 : Tyan Manufactures ID
        Byte (5) : Current Duty Cycle , present if 0xFE passed to Duty Cycle in Request
*/
ipmi::RspType<uint8_t> ipmi_tyan_FanPwmDuty(uint8_t pwmId, uint8_t duty)
{
    std::fstream file;
    int rc=0;
    char command[100];
    char temp[50];
    uint8_t responseDuty;
    uint8_t pwmValue = 0;

    if (duty == 0xfe)
    {
        // Get current duty cycle
        switch (pwmId)
        {
            case 0:
                    memset(command,0,sizeof(command));
                    sprintf(command, "cat /sys/class/hwmon/hwmon0/pwm1");
                    break;
            case 1:
                    memset(command,0,sizeof(command));
                    sprintf(command, "cat /sys/class/hwmon/hwmon0/pwm2");
                    break;
            case 2:
                    memset(command,0,sizeof(command));
                    sprintf(command, "cat /sys/class/hwmon/hwmon0/pwm3");
                    break;
            case 3:
                    memset(command,0,sizeof(command));
                    sprintf(command, "cat /sys/class/hwmon/hwmon0/pwm4");
                    break;
            case 4:
                    memset(command,0,sizeof(command));
                    sprintf(command, "cat /sys/class/hwmon/hwmon0/pwm5");
                    break;
            default:
                    return ipmi::responseParmOutOfRange();
        }

        memset(temp, 0, sizeof(temp));
        rc = execmd((char *)command, temp);

        if (rc != 0)
        {
            return ipmi::responseUnspecifiedError();
        }

        pwmValue = strtol(temp,NULL,10);
        responseDuty = pwmValue*100/255;
        return ipmi::responseSuccess(responseDuty);
    }
    else if (duty == 0xff)
    {
        // Return to automatic control
        memset(command,0,sizeof(command));
        sprintf(command, "systemctl start phosphor-pid-control.service");
        rc = system(command);

        memset(command,0,sizeof(command));
        sprintf(command, "echo 1 > /usr/sbin/fsc");
        system(command);
    }
    else if (duty <= 0x64)
    {
        memset(command,0,sizeof(command));
        sprintf(command, "systemctl stop phosphor-pid-control.service");
        rc = system(command);

        memset(command,0,sizeof(command));
        sprintf(command, "echo 0 > /usr/sbin/fsc");
        system(command);

        // control duty cycle (0%-100%)
        pwmValue = duty*255/100;

        switch (pwmId)
        {
            case 0:
                    memset(command,0,sizeof(command));
                    sprintf(command, "echo %d > /sys/class/hwmon/hwmon0/pwm1", pwmValue);
                    break;
            case 1:
                    memset(command,0,sizeof(command));
                    sprintf(command, "echo %d > /sys/class/hwmon/hwmon0/pwm2", pwmValue);
                    break;
            case 2:
                    memset(command,0,sizeof(command));
                    sprintf(command, "echo %d > /sys/class/hwmon/hwmon0/pwm3", pwmValue);
                    break;
            case 3:
                    memset(command,0,sizeof(command));
                    sprintf(command, "echo %d > /sys/class/hwmon/hwmon0/pwm4", pwmValue);
                    break;
            case 4:
                    memset(command,0,sizeof(command));
                    sprintf(command, "echo %d > /sys/class/hwmon/hwmon0/pwm5", pwmValue);
                    break;
            default:
                    return ipmi::responseParmOutOfRange();
        }
        rc = system(command);
    }
    else
    {
        return ipmi::responseParmOutOfRange();
    }

    if (rc != 0)
    {
        return ipmi::responseUnspecifiedError();
    }

    return ipmi::responseSuccess();
}
//===============================================================
/* Set Floor Duty Command
NetFun: 0x2E
Cmd : 0x07
Request:
        Byte 1-3 : Tyan Manufactures ID (FD 19 00)
        Byte 4 :  0h~64h - Floor Duty cycle
                     ffh - Get Current Floor Duty Cycle
Response:
        Byte 1 : Completion Code
        Byte 2-4 : Tyan Manufactures ID
        Byte (5) : Current Floor Duty Cycle , present if FFh passed to Byte4 in Request
        	[7] : Floor Duty Cycle control source 
        		0b - by sensor.
        		1b - by command.
        	[6:0] : Floor Duty cycle.
*/
ipmi::RspType<uint8_t> ipmi_tyan_FloorDuty(uint8_t floorDuty)
{
    int rc=0;
    char Object[100];
	double responseData;
    uint8_t currentFloorDuty;
	uint8_t controlSource;
	
	auto bus = sdbusplus::bus::new_default();

    if (floorDuty <= 0x64)
    {
        //Set Floor Duty Cycle
		for(size_t i = 1; i <= 5; i++)
		{
			memset(Object,0,sizeof(Object));
			snprintf(Object,sizeof(Object),"%s%d",FSC_OBJECTPATH,i);
			setProperty(bus,Object,"CommandSet",1);
			setProperty(bus,Object,"FloorDuty",static_cast<double>(floorDuty));
		}
    }
    else if (floorDuty == 0xff)
    {
        // Get Floor Duty Cycle
		memset(Object,0,sizeof(Object));
		snprintf(Object,sizeof(Object),"%s%d",FSC_OBJECTPATH,1);
		
		rc = getProperty(bus,Object,"FloorDuty",responseData,FSC_SERVICE,PID_INTERFACE);
		if(rc<0)
		{
			return ipmi::responseUnspecifiedError();
		}

		currentFloorDuty = static_cast<uint8_t>(responseData);
		
		rc = getProperty(bus,Object,"CommandSet",responseData,FSC_SERVICE,PID_INTERFACE);
		if(rc<0)
		{
			return ipmi::responseUnspecifiedError();
		}

		controlSource = static_cast<uint8_t>(responseData);
		if (controlSource == 1)
		{
			currentFloorDuty = currentFloorDuty + 0x80;
		}
		
		return ipmi::responseSuccess(currentFloorDuty);

    }
	else if (floorDuty == 0xfe)
	{
		//Set Floor Duty Cycle control by sensors 
		for(size_t i = 1; i <= 5; i++)
		{
			memset(Object,0,sizeof(Object));
			snprintf(Object,sizeof(Object),"%s%d",FSC_OBJECTPATH,i);
			setProperty(bus,Object,"CommandSet",0);
		}
	}
	else
	{
		return ipmi::responseParmOutOfRange();
	}
    
    return ipmi::responseSuccess();
}
//===============================================================
/* Config EccLeaky Bucket Command
NetFun: 0x2E
Cmd : 0x1A
Request:
        Byte 1-3 : Tyan Manufactures ID (FD 19 00)
        Byte (4) : optional, set T1 
        Byte (5) : optional, set T2 

Response:
        Byte 1 : Completion Code
        Byte 2-4 : Tyan Manufactures ID
        Byte (5) : Return current T1 if request length = 3. 
        Byte (6) : Return current T2 if request length = 3.
*/
ipmi::RspType<std::optional<uint8_t>, // T1 
              std::optional<uint8_t>  // T2
              >
    ipmi_tyan_ConfigEccLeakyBucket(std::optional<uint8_t> T1, std::optional<uint8_t> T2)
{

    constexpr const char* leakyBucktPath =
        "/xyz/openbmc_project/sensors/leakyBucket/HOST_DIMM_ECC";
    constexpr const char* leakyBucktIntf =
        "xyz.openbmc_project.Sensor.Value";
    std::shared_ptr<sdbusplus::asio::connection> busp = getSdBus();
    uint8_t t1;
    uint8_t t2;
    ipmi::Value result;

    auto service = ipmi::getService(*busp, leakyBucktIntf, leakyBucktPath);
    
    //get t1,t2
    if(!T1) 
    {
        try
        {
            result = ipmi::getDbusProperty(
                    *busp, service, leakyBucktPath, leakyBucktIntf, "T1");    
            t1 = std::get<uint8_t>(result);

            result = ipmi::getDbusProperty(
                *busp, service, leakyBucktPath, leakyBucktIntf, "T2");
            t2 = std::get<uint8_t>(result);
        
        }
        catch (const std::exception& e)
        {
            return ipmi::responseUnspecifiedError();
        }
        return ipmi::responseSuccess(t1,t2);
    }

    //t1 found 
    try
    {
       ipmi::setDbusProperty(
                *busp, service, leakyBucktPath, leakyBucktIntf, "T1", T1.value());    
    }
    catch (const std::exception& e)
    {
        return ipmi::responseUnspecifiedError();
    }

    if(T2)
    {
        try
        {
            ipmi::setDbusProperty(
                    *busp, service, leakyBucktPath, leakyBucktIntf, "T2", T2.value());    
        }
        catch (const std::exception& e)
        {
            return ipmi::responseUnspecifiedError();
        }
    }
    return ipmi::responseSuccess();
   
}

//===============================================================
/* get gpio status Command (for manufacturing) 
NetFun: 0x2E
Cmd : 0x41
Request:
        Byte 1-3 : Tyan IANA ID (FD 19 00)
        Byte 4 : gpio number 
Response:
        Byte 1 : Completion Code
        Byte 2-4 : Tyan IANA
        Byte 5 : gpio direction 
        Byte 6 : gpio data
*/
ipmi::RspType<uint8_t,
              uint8_t>
    ipmi_tyan_getGpio(uint8_t gpioNo)
{
    auto chip = gpiod::chip("gpiochip0");
    auto line = chip.get_line(gpioNo);
    
    if (!line)
    {
        std::cerr << "Error requesting gpio\n";
        return ipmi::responseUnspecifiedError();
    }
    auto dir = line.direction();

    bool resp;
    try
    {
        line.request({"ipmid", gpiod::line_request::DIRECTION_INPUT,0});
        resp = line.get_value();
    }
    catch (std::system_error&)
    {
        std::cerr << "Error reading gpio: " << (unsigned)gpioNo << "\n";
        return ipmi::responseUnspecifiedError();
    }
  
    line.release();
    return ipmi::responseSuccess((uint8_t)dir, (uint8_t)resp);
}
void register_netfn_mct_oem()
{
    ipmi_register_callback(NETFUN_TWITTER_OEM, IPMI_CMD_ClearCmos, NULL, ipmiOpmaClearCmos, PRIVILEGE_ADMIN);
    ipmi::registerOemHandler(ipmi::prioMax, 0x0019fd, IPMI_CMD_FanPwmDuty, ipmi::Privilege::Admin, ipmi_tyan_FanPwmDuty);
    ipmi::registerOemHandler(ipmi::prioMax, 0x0019fd, IPMI_CMD_ManufactureMode, ipmi::Privilege::Admin, ipmi_tyan_ManufactureMode);
	ipmi::registerOemHandler(ipmi::prioMax, IANA_TYAN, IPMI_CMD_FloorDuty, ipmi::Privilege::Admin, ipmi_tyan_FloorDuty);
    ipmi::registerOemHandler(ipmi::prioMax, IANA_TYAN, IPMI_CMD_ConfigEccLeakyBucket, ipmi::Privilege::Admin, ipmi_tyan_ConfigEccLeakyBucket);
    ipmi::registerOemHandler(ipmi::prioMax, IANA_TYAN, IPMI_CMD_gpioStatus, ipmi::Privilege::Admin, ipmi_tyan_getGpio);
}
}
