#pragma once

#include <iostream>
#include <vector>
#include <variant>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <unistd.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/server.hpp>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <xyz/openbmc_project/State/Host/server.hpp>


namespace fs = std::filesystem;
using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::State::server;

constexpr auto MILLI_OFFSET = 1000;
constexpr auto MICRO_OFFSET = 1000000;
constexpr auto MAX_COLLECTION_POWER_SIZE = 65535;

constexpr auto DCMI_SERVICE = "xyz.openbmc_project.DCMI";
constexpr auto DCMI_POWER_PATH = "/xyz/openbmc_project/DCMI/Power";
constexpr auto DCMI_POWER_INTERFACE = "xyz.openbmc_project.DCMI.Value";
constexpr auto PCAP_PATH = "/xyz/openbmc_project/control/host0/power_cap";
constexpr auto PCAP_INTERFACE = "xyz.openbmc_project.Control.Power.Cap";

constexpr auto PERIOD_MAX_PROP = "PeriodMaxValue";
constexpr auto PERIOD_MIN_PROP = "PeriodMinValue";
constexpr auto PERIOD_AVERAGE_PROP = "PeriodAverageValue";
constexpr auto POWER_CAP_PROP = "PowerCap";
constexpr auto POWER_CAP_ENABLE_PROP = "PowerCapEnable";
constexpr auto EXCEPTION_ACTION_PROP = "ExceptionAction";
constexpr auto CORRECTION_TIME_PROP = "CorrectionTime";
constexpr auto SAMPLING_PERIOD_PROP = "SamplingPeriod";

constexpr std::chrono::microseconds DBUS_TIMEOUT = std::chrono::microseconds(5*1000000);

typedef struct{
    double time = 0;
    unsigned int value = 0;
}Power;

typedef struct{
    double max=0;
    double min=0;
    double average=0;
    bool powerCapEnable=false;
    bool actionEnable=true;
    uint16_t samplingPeriod=0;
    uint32_t correctionTime=0;
    uint32_t correctionTimeout=0;
    uint32_t powerCap=0;
    std::string exceptionAction="";
    std::string powerPath="";
    std::vector<Power> collectedPower;
}PowerStore;

class PSUProperty
{
  public:
    PSUProperty(std::string name, double max, double min, unsigned int factor) :
        labelTypeName(name), maxReading(max), minReading(min),
        sensorScaleFactor(factor)
    {
    }
    ~PSUProperty() = default;

    std::string labelTypeName;
    double maxReading;
    double minReading;
    unsigned int sensorScaleFactor;
};

static boost::container::flat_map<std::string,PSUProperty> labelMatch;

/** @brief Read the value from hardware monitor path.
 *
 *  @param[in] path - hardware monitor path for power.
 */
int readPower(std::string path);

/** @brief Update current power value to dbus property.
 *
 *  @param[in] property - Dbus property.
 *  @param[in] newValue - Current power value.
 *  @param[in] oldValue - Stored power value.
 */
void updatePowerValue(std::string property,const double& newValue,double& oldValue);

/** @brief Initialize the property for match power label and power which stored at dbus.
 *
 *  @param[in] powerStore - Stored power struct.
 */
void propertyInitialize(PowerStore& powerStore);

/** @brief Initialize the dbus Service for dcmi power.
 *
 *  @param[in] powerStore - Stored power struct.
 */
void dbusServiceInitialize(PowerStore& powerStore);

/** @brief Find the hardware monitor path for power.
 *
 *  @return On success returns path with stored power.
 */
std::string findPowerPath();

/**
 * @brief Handle the DCMI power action
 *
 * @param[in] io - io context.
 * @param[in] delay - Delay time in micro second.
 */
void powerHandler(boost::asio::io_context& io,PowerStore& powerStore,double delay);

/** @brief Handle the correction timeout for power limit.
 *
 *  @param[in] currentPower - Current power with time and value.
 *  @param[in] powerStore - Stored power struct.
 */
void timeoutHandle(Power currentPower,PowerStore& powerStore);
