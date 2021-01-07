#include <sdbusplus/bus.hpp>
#include <sdbusplus/bus/match.hpp>
#include <sdbusplus/message.hpp>
#include <sdbusplus/vtable.hpp>
#include <sdbusplus/server/interface.hpp>
#include <peci.h>
#include <systemd/sd-journal.h>
#include <iostream>
#include <variant>


#define CATERR_OBJECTPATH_BASE "/xyz/openbmc_project/control/processor"
#define CATERR_SERVICE "xyz.openbmc_project.Settings"
#define CATERR_INTERFACE "xyz.openbmc_project.Control.Processor"
#define PROPERTY_INTERFACE "org.freedesktop.DBus.Properties"

static constexpr bool DEBUG = true;

static constexpr char const* ipmiSELService =    "xyz.openbmc_project.Logging.IPMI";
static constexpr char const* ipmiSELPath = "/xyz/openbmc_project/Logging/IPMI";
static constexpr char const* ipmiSELAddInterface = "xyz.openbmc_project.Logging.IPMI";

static constexpr char const *crashdumpService = "com.intel.crashdump";
static constexpr char const *crashdumpPath = "/com/intel/crashdump";
static constexpr char const *crashdumpIntf = "com.intel.crashdump.Stored";

static constexpr char const* caterrSensorPath = "/xyz/openbmc_project/sensors/processor/CATERR";
static constexpr char const* cpu0SensorPath = "/xyz/openbmc_project/sensors/processor/CPU0_State";
static constexpr char const* cpu1SensorPath = "/xyz/openbmc_project/sensors/processor/CPU1_State";

static constexpr char const* powerService = "xyz.openbmc_project.State.Host";
static constexpr char const* powerPath = "/xyz/openbmc_project/state/host0";
static constexpr char const* powerInterface = "xyz.openbmc_project.State.Host";

static const std::string ipmiSELAddMessage = "SEL Entry";
static constexpr size_t selEvtDataMaxSize = 3;
static const uint32_t intenalErrMask = 0x10100000; //internal CATERR or MSMI
static const uint32_t ierrMask = 0x48480000; 
static const uint32_t mcerrMask = 0x24240000; 

int main()
{

    auto bus = sdbusplus::bus::new_default();
#if 0      //ditch dbus property change
    std::string state="xyz.openbmc_project.Control.Processor.State.CATERR"; 
    auto method = bus.new_method_call(CATERR_SERVICE, CATERR_OBJECTPATH_BASE,
                                      PROPERTY_INTERFACE, "Set");
    method.append(CATERR_INTERFACE,"ProcessorStatus", sdbusplus::message::variant<std::string>(state));
    bus.call_noreply(method);
#endif 

#ifdef CATERR_ENABLE_POWER_FILTER
    auto method = bus.new_method_call(powerService, powerPath, PROPERTY_INTERFACE,"Get");
    method.append(powerInterface, "CurrentHostState");

    std::variant<std::string> result;
    try
    {
        auto reply = bus.call(method);
        reply.read(result);
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR=" << e.what() << "\n";
        return 0;
    }

    std::string powerState = std::get<std::string>(result);
    if (powerState.find("Running") == std::string::npos)
    {
        std::cerr << "Filter out CATERR event\n";
        return 0;
    }
#endif

    //log SEL
    uint16_t genId = 0x20;
    bool assert=1;
    std::vector<uint8_t> eventData(selEvtDataMaxSize, 0xFF);
    uint32_t mcaErrorSrc = 0xFFFFFFFF;

    uint8_t cc = 0;
    std::string errorType = "IERR";
    std::string errorSenorPath;

    //PECI fail, log CATERR SEL
    if (peci_RdPkgConfig(0x30, PECI_MBX_INDEX_CPU_ID,PECI_PKG_ID_MACHINE_CHECK_STATUS, sizeof(uint32_t),
                                reinterpret_cast<uint8_t*> (&mcaErrorSrc), &cc) != PECI_CC_SUCCESS)
    
    {
        eventData[0] = static_cast<uint8_t>(0xb);  
        errorSenorPath = caterrSensorPath;
        sdbusplus::message::message writeSEL = bus.new_method_call(
                        ipmiSELService, ipmiSELPath, ipmiSELAddInterface, "IpmiSelAdd");
        writeSEL.append(ipmiSELAddMessage,  errorSenorPath, eventData, assert, genId);
                      
        try
        {
            bus.call(writeSEL);
        }
        catch (sdbusplus::exception_t& e)
        {
            std::cerr << "call IpmiSelAdd failed\n";
        }
        return 0;
    }
    
    sd_journal_print(LOG_ERR, "mcaErrorSrc = 0x%08x\n", mcaErrorSrc);    
    //false postive, ignore it 
    if ( 0 == mcaErrorSrc)
    {
        return 0;
    }
    //determine error socket
    errorSenorPath = (mcaErrorSrc & intenalErrMask) ? cpu0SensorPath : cpu1SensorPath;
                                                                    
    
    //log mcerr
    if (mcaErrorSrc & mcerrMask)
    {
        eventData[0] = 0xc;
        errorType = "MCERR";
    
        sdbusplus::message::message writeMcerrSEL = bus.new_method_call(
                     ipmiSELService, ipmiSELPath, ipmiSELAddInterface, "IpmiSelAdd");
        writeMcerrSEL.append(ipmiSELAddMessage,  errorSenorPath, eventData, assert, genId);
                      
        try
        {
            bus.call(writeMcerrSEL);
        }
        catch (sdbusplus::exception_t& e)
        {
            std::cerr << "Failed to log MCERR SEL\n";
        }     
    }
    
    //log ierr
    if (mcaErrorSrc & ierrMask)
    {
        eventData[0] = 0x00;
        errorType = "IERR";
    
        sdbusplus::message::message writeIerrSEL = bus.new_method_call(
                                ipmiSELService, ipmiSELPath, ipmiSELAddInterface, "IpmiSelAdd");
        writeIerrSEL.append(ipmiSELAddMessage,  errorSenorPath, eventData, assert, genId);
                      
        try
        {
            bus.call(writeIerrSEL);
        }
        catch (sdbusplus::exception_t& e)
        {
            std::cerr << "Failed to log IERR SEL\n";
        }     
    }
                       
    // Crashdump
    sdbusplus::message::message crashdump = bus.new_method_call(
             crashdumpService, crashdumpPath, crashdumpIntf, "GenerateStoredLog");
    crashdump.append(errorType);
    
    try
    {
        bus.call_noreply(crashdump);
    }
    catch (sdbusplus::exception_t& e)
    {
        std::cerr<<"Failed to call crashdump\n";
    }
    
    return 0;

}
