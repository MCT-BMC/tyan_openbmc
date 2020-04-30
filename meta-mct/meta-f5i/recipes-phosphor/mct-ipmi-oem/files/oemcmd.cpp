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
#include <boost/algorithm/string.hpp>

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
#include <phosphor-logging/elog-errors.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/message/types.hpp>
#include <peci.h>

#include "xyz/openbmc_project/Control/Power/RestorePolicy/server.hpp"

#define FSC_SERVICE "xyz.openbmc_project.EntityManager"
#define FSC_OBJECTPATH "/xyz/openbmc_project/inventory/system/board/s7106_Baseboard/Pid_"
#define PID_INTERFACE "xyz.openbmc_project.Configuration.Pid.Zone"
#define PROPERTY_INTERFACE "org.freedesktop.DBus.Properties"

#include<filesystem>

using phosphor::logging::level;
using phosphor::logging::log;

using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Control::Power::server;

namespace fs = std::filesystem;

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
    char Object[100];
    auto bus = sdbusplus::bus::new_default();

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

        //Set Floor Duty Cycle control by sensors
        for(size_t i = 1; i <= 5; i++)
        {
            memset(Object,0,sizeof(Object));
            snprintf(Object,sizeof(Object),"%s%d",FSC_OBJECTPATH,i);
            setProperty(bus,Object,"CommandSet",0);
        }
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

//===============================================================
/* Power Node Manager Oem Get Reading
NetFun: 0x30
Cmd : 0xE2
Request:
        Byte 1 : [0:3] Domain ID / [4:7] Reading Type
        Byte 2 : Optional Parameter.
        Byte 3 : Reserved. Write as 00h.

Response:
        Byte 1 : Completion Code
        Byte 2 : [0:3] Domain ID / [4:7] Reading Type
        Byte 3-4 : Reading value 16-bit encoding 2s-complement signed integer.
*/

#define PIN_OBJECT "/xyz/openbmc_project/sensors/power/PSU0_Input_Power"
#define PIN_SERVICE "xyz.openbmc_project.PSUSensor"
#define PIN_INTERFACE "xyz.openbmc_project.Sensor.Value"

ipmi_ret_t ipmi_Pnm_GetReading(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                              ipmi_request_t request, ipmi_response_t response,
                              ipmi_data_len_t data_len, ipmi_context_t context)
{
    ipmi_ret_t ipmi_rc = IPMI_CC_OK;
    uint8_t domainID;
    uint8_t readingType;
    uint8_t highByte;
    uint8_t lowByte;
    int rc=0;
    double readingValue;
    uint8_t responseData[3]={0};

    auto bus = sdbusplus::bus::new_default();
    auto* requestData= reinterpret_cast<PnmGetReadingRequest*>(request);

    if((int)*data_len != 3)
    {
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }

    domainID = requestData->type & 0x0F;
    readingType = requestData->type >> 4;
    if (readingType == 0x00 || readingType == 0x06)
    {
        // get PSU PIN sensor reading value
        rc = getProperty(bus,PIN_OBJECT,"Value",readingValue,PIN_SERVICE,PIN_INTERFACE);
        if(rc<0)
        {
            return IPMI_CC_UNSPECIFIED_ERROR;
        }

        highByte = static_cast<uint16_t>(readingValue) >> 8;
        lowByte = static_cast<uint16_t>(readingValue) & 0x00FF;
        responseData[1]=lowByte;
        responseData[2]=highByte;
    }

    responseData[0]=requestData->type;
    memcpy(response, responseData, sizeof(responseData));

    return ipmi_rc;
}

//===============================================================
/* Set FRU Field Command
NetFun: 0x2E
Cmd : 0x0B
Request:
    Byte 1-3 : Tyan Manufactures ID (FD 19 00)
    Byte 4 :  FRU ID
    Byte 5 :  Item
        [7:4] : Area (1-Chassis Info , 2-Board Info , 3-Product Info)
        [3:0] : Field
    Byte 6-N : Data
Response:
    Byte 1 : Completion Code
    Byte 2-4 : Tyan Manufactures ID
*/
ipmi::RspType<> ipmi_setFruField(uint8_t fruId, uint4_t field, uint4_t area, std::vector<uint8_t> dataInfo)

{
    std::string s;
    size_t pos;
    char command[100];
    char cmd[100];
    char string[100];

    if(fruId != 0)
    {
        // Currently, only support FRU ID 0
        return ipmi::responseReqDataLenInvalid();
    }

    for(int i=0; i<dataInfo.size(); i++)
    {
        s += (char)dataInfo[i];
    }

    strcpy(string,s.c_str());
    memset(command,0,sizeof(command));

    switch ((uint8_t)area)
    {
        case 0:
            // flag
            snprintf(command,sizeof(command),"echo %d > /usr/sbin/fruFlag",dataInfo[0]);

            if(dataInfo[0] == 2)
            {
                memset(cmd,0,sizeof(cmd));
                snprintf(cmd,sizeof(cmd),"/usr/sbin/writeFRU.sh &",dataInfo[0]);
                system(cmd);
            }
            break;
        case 1:
            // chassis information are
            snprintf(command,sizeof(command),"echo c %d %s >> /usr/sbin/fruWrite",(int)field,string);
            break;
        case 2:
            // board information area
            snprintf(command,sizeof(command),"echo b %d %s >> /usr/sbin/fruWrite",(int)field,string);
            break;
        case 3:
            // product information area
            snprintf(command,sizeof(command),"echo p %d %s >> /usr/sbin/fruWrite",(int)field,string);
            break;
        default:
            return ipmi::responseParmOutOfRange();
    }

    system(command);
    return ipmi::responseSuccess();
}

std::string getFruData()
{
    std::string str;
    char command[100];

    if(fs::exists("/usr/sbin/fruData"))
    {
        std::ifstream file("/usr/sbin/fruData", std::ios::in);

        auto data = std::vector<uint8_t>(std::istreambuf_iterator<char>(file),
                                     std::istreambuf_iterator<char>());
        if (file.fail())
        {
            std::cout << "read FRU Data fail" << std::endl;
        }
        file.close();

        for(int i=0; i<data.size(); i++)
        {
            str += (char)data[i];
        }
    }
    else
    {
        memset(command,0,sizeof(command));
        sprintf(command, "ipmitool fru print 0 > /usr/sbin/fruData &");
        system(command);
    }
    return str;
}

std::string areaData(std::string str, std::string str1, std::string str2)
{
    std::string string;
    std::string::size_type pos,pos1;
    std::string STR;

    pos = str.find(str1);
    if(pos != std::string::npos)
    {
        string = str.substr(pos);
        pos = string.find(":");
        pos1 = string.find(str2);
        if(pos1 != std::string::npos)
        {
            STR = string.substr(pos+2,(pos1-pos-3));
        }
        else
        {
            STR = string.substr (pos+2);
        }
    }
    return STR;
}

//===============================================================
/* Get FRU Field Command
NetFun: 0x2E
Cmd : 0x0C
Request:
    Byte 1-3 : Tyan Manufactures ID (FD 19 00)
    Byte 4 :  FRU ID
    Byte 5 :  Item
        [7:4] : Area (1-Chassis Info , 2-Board Info , 3-Product Info)
        [3:0] : Field
Response:
    Byte 1 : Completion Code
    Byte 2-4 : Tyan Manufactures ID
    Byte 5-N : Data
*/
ipmi::RspType<std::vector<uint8_t>> ipmi_getFruField(uint8_t fruId, uint4_t field, uint4_t area)
{

    int rc=0;
    char command[100],temp[50];
    char string[100];
    size_t pos;
    std::string str,str1,str2;
    uint8_t f;

    if(fruId != 0)
    {
        // Currently, only support FRU ID 0
        return ipmi::responseReqDataLenInvalid();
    }

    if(area > 0x3)
    {
        //fail Area
        return ipmi::responseInvalidFieldRequest();
    }
    else if(area == 0x0)
    {
        //flag
        if(fs::exists("/usr/sbin/fruFlag"))
        {
            memset(command,0,sizeof(command));
            memset(temp, 0, sizeof(temp));
            sprintf(command, "cat /usr/sbin/fruFlag");
            rc = execmd((char *)command, temp);
            if (rc != 0)
            {
                return ipmi::responseUnspecifiedError();
            }

            f = strtol(temp,NULL,10);
            std::vector<uint8_t> flag = {f};
            return ipmi::responseSuccess(flag);
        }
        else
        {
            return ipmi::responseInvalidFieldRequest();
        }
    }
    else
    {
        str=getFruData();

        switch ((uint8_t)area)
        {
            case 1:
                // chassis information are
                switch((uint8_t)field)
                {
                    case 0:
                        // chassis type
                        str2 = areaData(str,"Chassis Type","Chassis Part");
                        break;
                    case 1:
                        // chassis part number
                        str2 = areaData(str,"Chassis Part","Chassis Serial");
                        break;
                    case 2:
                        // chassis serial umber
                        str2 = areaData(str,"Chassis Serial","Board Mfg Date");
                        break;
                    default:
                        break;
                }
                break;
            case 2:
                // board information area
                pos = str.find("Board Mfg Date");
                str1 = str.substr(pos+14);
                switch((uint8_t)field)
                {
                    case 0:
                        // board manufacturer
                        str2 = areaData(str1,"Board Mfg","Board Product");
                        break;
                    case 1:
                        // board product name
                        str2 = areaData(str1,"Board Product","Board Serial");
                        break;
                    case 2:
                        // board serial number
                        str2 = areaData(str1,"Board Serial","Board Part");
                        break;
                    case 3:
                        // board part number
                        str2 = areaData(str1,"Board Part","Product Manufacturer");
                        break;
                    default:
                        break;
                }
                break;
            case 3:
                // product information area
                switch((uint8_t)field)
                {
                    case 0:
                        // product manufacturer
                        str2 = areaData(str,"Product Manufacturer","Product Name");
                        break;
                    case 1:
                        // product name
                        str2 = areaData(str,"Product Name","Product Part");
                        break;
                    case 2:
                        // product part / model number
                        str2 = areaData(str,"Product Part","Product Version");
                        break;
                    case 3:
                        // product version
                        str2 = areaData(str,"Product Version","Product Serial");
                        break;
                    case 4:
                        // product serial number
                        str2 = areaData(str,"Product Serial","Product Asset");
                        break;
                    default:
                        break;
                }
                break;
        }
    }

    if((char)str2[str2.length()-1] == 0xa)
    {
        str2.pop_back();
    }

    std::vector<uint8_t> DATA(str2.begin(), str2.end());
    return ipmi::responseSuccess(DATA);
}

//===============================================================
/* Get Firmware String Command
NetFun: 0x2E
Cmd : 0x10
Request:
    Byte 1-3 : Tyan Manufactures ID (FD 19 00)
Response:
    Byte 1 : Completion Code
    Byte 2-4 : Tyan Manufactures ID
    Byte 5-N : Firmware String
*/
ipmi::RspType<std::vector<uint8_t>> ipmi_getFirmwareString()
{
    std::vector<uint8_t>firmwareString(3,0);
    std::string osReleasePath = "/etc/os-release";
    std::string searchFirmwareString = "OPENBMC_TARGET_MACHINE";
    std::string readText;
    std::ifstream readFile(osReleasePath);

    while(getline(readFile, readText)) {
        std::size_t found = readText.find(searchFirmwareString);
        if (found!=std::string::npos){
            std::vector<std::string> readTextSplite;
            boost::split(readTextSplite, readText, boost::is_any_of( "\"" ) );
            firmwareString.assign(readTextSplite[1].length()+1, 0);
            std::copy(readTextSplite[1].begin(), readTextSplite[1].end(), firmwareString.begin());
        }
    }

    readFile.close();

    return ipmi::responseSuccess(firmwareString);
}

//===============================================================

ipmi::RspType<std::vector<uint8_t>>
    ipmi_sendRawPeci(uint8_t clientAddr, uint8_t writeLength, uint8_t readLength,
                        std::vector<uint8_t> writeData)
{
    if (readLength > PECI_BUFFER_SIZE)
    {
        log<level::ERR>("sendRawPeci command: Read length exceeds limit");
        return ipmi::responseParmOutOfRange();
    }
    std::vector<uint8_t> rawResp(readLength);
    if (peci_raw(clientAddr, readLength, writeData.data(), writeData.size(),
                             rawResp.data(), rawResp.size()) != PECI_CC_SUCCESS)
    {
        log<level::ERR>("sendRawPeci command: PECI command failed");
        return ipmi::responseResponseError();
    }

    return ipmi::responseSuccess(rawResp);

}

//===============================================================
/* Set/Get Ramdom Delay AC Restore Power ON Command
NetFun: 0x30
Cmd : 0x18
Request:
    Byte 1 : Op Code
        [7] : 0-Get 1-Set
        [6:0] :
            00h-Disable Delay
            01h-Enable Delay, Random Delay Time
            02h-Enable Delay, Fixed Delay Time
    Byte 2-3 : Delay Time, LSB first (Second base)
Response:
    Byte 1 : Completion Code
    Byte 2 :  Op Code
    Byte 3-4 : Delay Time, LSB first (Second base)
*/
ipmi::RspType<uint8_t, uint8_t, uint8_t> ipmi_tyan_RamdomDelayACRestorePowerON(uint8_t opCode, uint8_t delayTimeLSB,uint8_t delayTimeMSB)
{
    std::uint8_t opCodeResponse, delayTimeLSBResponse, delayTimeMSBResponse;;

    constexpr auto service = "xyz.openbmc_project.Settings";
    constexpr auto path = "/xyz/openbmc_project/control/host0/power_restore_policy";
    constexpr auto powerRestoreInterface = "xyz.openbmc_project.Control.Power.RestorePolicy";
    constexpr auto alwaysOnPolicy = "PowerRestoreAlwaysOnPolicy";
    constexpr auto delay = "PowerRestoreDelay";

    auto bus = sdbusplus::bus::new_default();

    if(opCode & 0x80)
    {
        //Set
        opCodeResponse = opCode;
        delayTimeLSBResponse = delayTimeLSB;
        delayTimeMSBResponse = delayTimeMSB;
        try
        {
            auto method = bus.new_method_call(service, path, PROPERTY_INTERFACE,"Set");
            method.append(powerRestoreInterface, alwaysOnPolicy, sdbusplus::message::variant<std::string>(RestorePolicy::convertAlwaysOnPolicyToString((RestorePolicy::AlwaysOnPolicy)(opCode & 0x7F))));
            bus.call_noreply(method);

            uint32_t delayValue = delayTimeLSB | (delayTimeMSB << 8);

            auto methodDelay = bus.new_method_call(service, path, PROPERTY_INTERFACE, "Set");
            methodDelay.append(powerRestoreInterface, delay, sdbusplus::message::variant<uint32_t>(delayValue));
            bus.call_noreply(methodDelay);
        }
        catch (const sdbusplus::exception::SdBusError& e)
        {
            log<level::ERR>("Error in RamdomDelayACRestorePowerON Set",entry("ERROR=%s", e.what()));
            return ipmi::responseParmOutOfRange();
        }
    }
    else
    {
        //Get
        auto method = bus.new_method_call(service, path, PROPERTY_INTERFACE,"Get");
        method.append(powerRestoreInterface, alwaysOnPolicy);

        sdbusplus::message::variant<std::string> result;
        try
        {
            auto reply = bus.call(method);
            reply.read(result);
        }
        catch (const sdbusplus::exception::SdBusError& e)
        {
            log<level::ERR>("Error in PowerRestoreAlwaysOnPolicy Get",entry("ERROR=%s", e.what()));
            return ipmi::responseUnspecifiedError();
        }
        auto powerAlwaysOnPolicy = sdbusplus::message::variant_ns::get<std::string>(result);

        auto methodDelay = bus.new_method_call(service, path, PROPERTY_INTERFACE, "Get");
        methodDelay.append(powerRestoreInterface, delay);

        sdbusplus::message::variant<uint32_t> resultDelay;
        try
        {
            auto reply = bus.call(methodDelay);
            reply.read(resultDelay);
        }
        catch (const sdbusplus::exception::SdBusError& e)
        {
            log<level::ERR>("Error in PowerRestoreDelay Get",entry("ERROR=%s", e.what()));
            return ipmi::responseUnspecifiedError();
        }
        auto powerRestoreDelay = sdbusplus::message::variant_ns::get<uint32_t>(resultDelay);

        opCodeResponse = (opCode & 0x80) | (uint8_t)RestorePolicy::convertAlwaysOnPolicyFromString(powerAlwaysOnPolicy);

        uint8_t *delayValue = (uint8_t *)&powerRestoreDelay;
        delayTimeLSBResponse = delayValue[0];
        delayTimeMSBResponse = delayValue[1];
    }

    return ipmi::responseSuccess(opCodeResponse,delayTimeLSBResponse,delayTimeMSBResponse);
}

//===============================================================
/* Set specified service enable or disable
NetFun: 0x30
Cmd : 0x0D
Request:
    Byte 1 : Set service status
        [7-1] : reserved
        [0] :
            0h-Disable web service
            1h-Enable web service
Response:
    Byte 1 : Completion Code
*/
ipmi::RspType<> ipmi_SetService(uint8_t serviceSetting)
{
    constexpr auto service = "xyz.openbmc_project.Settings";
    constexpr auto path = "/xyz/openbmc_project/oem/ServiceStatus";
    constexpr auto serviceStatusInterface = "xyz.openbmc_project.OEM.ServiceStatus";
    constexpr auto webService = "WebService";

    auto bus = sdbusplus::bus::new_default();

    //Set web service status
    try
    {
        auto method = bus.new_method_call(service, path, PROPERTY_INTERFACE,"Set");
        method.append(serviceStatusInterface, webService, sdbusplus::message::variant<bool>(serviceSetting & 0x01));
        bus.call_noreply(method);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("Error in RamdomDelayACRestorePowerON Set",entry("ERROR=%s", e.what()));
        return ipmi::responseParmOutOfRange();
    }

    return ipmi::responseSuccess();
}

//===============================================================
/* Get specified service enable or disable status
NetFun: 0x30
Cmd : 0x0E
Request:

Response:
    Byte 1 : Completion Code
    Byte 2 : Set service status
        [7-1] : reserved
        [0] :
            0h-Web service is disable
            1h-Web service is enable
*/
ipmi::RspType<uint8_t> ipmi_GetService()
{
    uint8_t serviceResponse = 0;

    constexpr auto service = "xyz.openbmc_project.Settings";
    constexpr auto path = "/xyz/openbmc_project/oem/ServiceStatus";
    constexpr auto serviceStatusInterface = "xyz.openbmc_project.OEM.ServiceStatus";
    constexpr auto webService = "WebService";

    auto bus = sdbusplus::bus::new_default();

    //Get web service status
    auto method = bus.new_method_call(service, path, PROPERTY_INTERFACE, "Get");
    method.append(serviceStatusInterface, webService);

    sdbusplus::message::variant<bool> result;
    try
    {
        auto reply = bus.call(method);
        reply.read(result);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("Error in PowerRestoreDelay Get",entry("ERROR=%s", e.what()));
        return ipmi::responseUnspecifiedError();
    }
    auto webServiceStatus = sdbusplus::message::variant_ns::get<bool>(result);

    serviceResponse = (uint8_t)(webServiceStatus);

    return ipmi::responseSuccess(serviceResponse);
}

void register_netfn_mct_oem()
{
    ipmi_register_callback(NETFUN_TWITTER_OEM, IPMI_CMD_ClearCmos, NULL, ipmiOpmaClearCmos, PRIVILEGE_ADMIN);
    ipmi_register_callback(NETFUN_TWITTER_OEM, IPMI_CMD_PnmGetReading, NULL, ipmi_Pnm_GetReading, PRIVILEGE_ADMIN);
    ipmi::registerOemHandler(ipmi::prioMax, 0x0019fd, IPMI_CMD_FanPwmDuty, ipmi::Privilege::Admin, ipmi_tyan_FanPwmDuty);
    ipmi::registerOemHandler(ipmi::prioMax, 0x0019fd, IPMI_CMD_ManufactureMode, ipmi::Privilege::Admin, ipmi_tyan_ManufactureMode);
	ipmi::registerOemHandler(ipmi::prioMax, IANA_TYAN, IPMI_CMD_FloorDuty, ipmi::Privilege::Admin, ipmi_tyan_FloorDuty);
    ipmi::registerOemHandler(ipmi::prioMax, IANA_TYAN, IPMI_CMD_ConfigEccLeakyBucket, ipmi::Privilege::Admin, ipmi_tyan_ConfigEccLeakyBucket);
    ipmi::registerOemHandler(ipmi::prioMax, IANA_TYAN, IPMI_CMD_gpioStatus, ipmi::Privilege::Admin, ipmi_tyan_getGpio);
    ipmi::registerOemHandler(ipmi::prioMax, IANA_TYAN, IPMI_CMD_SetFruField, ipmi::Privilege::Admin, ipmi_setFruField);
    ipmi::registerOemHandler(ipmi::prioMax, IANA_TYAN, IPMI_CMD_GetFruField, ipmi::Privilege::Admin, ipmi_getFruField);
    ipmi::registerOemHandler(ipmi::prioMax, IANA_TYAN, IPMI_CMD_GetFirmwareString, ipmi::Privilege::Admin, ipmi_getFirmwareString);
    ipmi::registerHandler(ipmi::prioMax, NETFUN_TWITTER_OEM, IPMI_CMD_SendRawPeci, ipmi::Privilege::Admin, ipmi_sendRawPeci);
    ipmi::registerHandler(ipmi::prioMax, NETFUN_TWITTER_OEM, IPMI_CMD_RamdomDelayACRestorePowerON, ipmi::Privilege::Admin, ipmi_tyan_RamdomDelayACRestorePowerON);
    ipmi::registerHandler(ipmi::prioMax, NETFUN_TWITTER_OEM, IPMI_CMD_SetService, ipmi::Privilege::Admin, ipmi_SetService);
    ipmi::registerHandler(ipmi::prioMax, NETFUN_TWITTER_OEM, IPMI_CMD_GetService, ipmi::Privilege::Admin, ipmi_GetService);
}
}
