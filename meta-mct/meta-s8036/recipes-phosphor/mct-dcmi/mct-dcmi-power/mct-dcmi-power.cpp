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

#include "mct-dcmi-power.hpp"

#include <iostream>
#include <xyz/openbmc_project/Control/Power/Cap/server.hpp>
#include "utils.hpp"

using PowerLimit = sdbusplus::xyz::openbmc_project::Control::Power::server::Cap;

static constexpr bool DEBUG = false;

int readPower(std::string path)
{
    std::fstream powerValueFile(path, std::ios_base::in);
    int value;
    int factor = 1000000;
    powerValueFile >> value;
    return value / factor;
}

void updatePowerValue(std::string property,const double& newValue,double& oldValue)
{
    if(newValue != oldValue)
    {
        oldValue = newValue;
        powerInterface->set_property(property, newValue);
    }
}

void propertyInitialize(PowerStore& powerStore)
{
    labelMatch = 
    {
        {"pout1", PSUProperty("Output Power", 3000, 0, 6)},
        {"pout2", PSUProperty("Output Power", 3000, 0, 6)},
        {"pout3", PSUProperty("Output Power", 3000, 0, 6)},
        {"power1", PSUProperty("Output Power", 3000, 0, 6)}
    };

    try
    {
        auto service = getService(bus, PCAP_INTERFACE, PCAP_PATH);
        auto properties = getAllDbusProperties(bus, service, PCAP_PATH,
                                                     PCAP_INTERFACE,DBUS_TIMEOUT);
        powerStore.powerCap = std::get<uint32_t>(properties[POWER_CAP_PROP]);
        powerStore.powerCapEnable = std::get<bool>(properties[POWER_CAP_ENABLE_PROP]);
        powerStore.exceptionAction = std::get<std::string>(properties[EXCEPTION_ACTION_PROP]);
        powerStore.correctionTime = std::get<uint32_t>(properties[CORRECTION_TIME_PROP]);
        powerStore.samplingPeriod = std::get<uint16_t>(properties[SAMPLING_PERIOD_PROP]);

    }
    catch (SdBusError& e)
    {
        log<level::ERR>("Error in initialize power cap parameter set",entry("ERROR=%s", e.what()));
        propertyInitialize(powerStore);
    }

}

void dbusServiceInitialize(PowerStore& powerStore)
{
    bus->request_name(DCMI_SERVICE);
    sdbusplus::asio::object_server objServer(bus);
    powerInterface=objServer.add_interface(DCMI_POWER_PATH,DCMI_POWER_INTERFACE);
    powerInterface->register_property(
        PERIOD_MAX_PROP, powerStore.max, sdbusplus::asio::PropertyPermission::readWrite);
    powerInterface->register_property(
        PERIOD_MIN_PROP, powerStore.min, sdbusplus::asio::PropertyPermission::readWrite);
    powerInterface->register_property(
        PERIOD_AVERAGE_PROP, powerStore.average, sdbusplus::asio::PropertyPermission::readWrite);
    powerInterface->initialize();
}

std::string findPowerPath()
{
    static std::vector<std::string> pmbusNames = {
    "isl68137", "ina219",   "ina230", "max20730", "max20734",
    "max20796", "max34451", "pmbus",  "pxe1610", "cffps1"};

    std::string powerPath = "";
    std::vector<fs::path> pmbusPaths;
    if (!findFiles(fs::path("/sys/class/hwmon"), "name", pmbusPaths, 1))
    {
        if constexpr (DEBUG)
        {
            std::cerr << "No PSU sensors in system\n";
        }
        return powerPath;
    }

    for (const auto& pmbusPath : pmbusPaths)
    {
        std::ifstream nameFile(pmbusPath);
        if (!nameFile.good())
        {
            if constexpr (DEBUG)
            {
                std::cerr << "Failure finding pmbus path " << pmbusPath << "\n";
            }
            continue;
        }
        std::string pmbusName;
        std::getline(nameFile, pmbusName);
        nameFile.close();

        if (std::find(pmbusNames.begin(), pmbusNames.end(), pmbusName) ==
            pmbusNames.end())
        {
            // To avoid this error message, add your driver name to
            // the pmbusNames vector at the top of this file.
            if constexpr (DEBUG)
            {
                std::cerr << "Driver name " << pmbusName << " not found in sensor whitelist\n";
            }
            continue;
        }
        const std::string* psuName;
        auto directory = pmbusPath.parent_path();

        std::vector<fs::path> sensorPaths;
        if (!findFiles(directory, R"(\w\d+_input$)", sensorPaths, 0))
        {
            if constexpr (DEBUG)
            {
                std::cerr << "No PSU non-label sensor in PSU\n";
            }
            continue;
        }
        for (const auto& sensorPath : sensorPaths)
        {
            std::string labelHead;
            std::string sensorPathStr = sensorPath.string();
            std::string sensorNameStr = sensorPath.filename();

            auto labelPath = boost::replace_all_copy(sensorPathStr, "input", "label");
            std::ifstream labelFile(labelPath);
            if (!labelFile.good())
            {
                if constexpr (DEBUG)
                {
                    std::cerr << "Input file " << sensorPath << " has no corresponding label file\n";
                }

                // hwmon *_input filename with number:
                // temp1, temp2, temp3, ...
                labelHead = sensorNameStr.substr(0, sensorNameStr.find("_"));
            }
            else
            {
                std::string label;
                std::getline(labelFile, label);
                labelFile.close();

                // hwmon corresponding *_label file contents:
                // vin1, vout1, ...
                labelHead = label.substr(0, label.find(" "));
            }

            auto findProperty = labelMatch.find(labelHead);
            if (findProperty == labelMatch.end())
            {
                if constexpr (DEBUG)
                {
                    std::cerr << "Could not find matching default property for " << labelHead << "\n";
                }
                continue;
            }

            if constexpr (DEBUG)
            {
                std::cerr << "Sensor name \"" << sensorNameStr << "\" path \"" << sensorPathStr << "\"\n";
            }

            powerPath = sensorPathStr;
            continue;
        }

    }
    return powerPath;
}

void powerHandler(boost::asio::io_context& io,PowerStore& powerStore,double delay)
{
    static boost::asio::steady_timer timer(io);

    timer.expires_after(std::chrono::microseconds((long)delay));

    timer.async_wait([&io,&powerStore](const boost::system::error_code&) 
    {
        double start,end;
        double max=0;
        double min=0;
        double average =0;
        double averageCount = 0;
        Power currentPower;

        if(powerStore.powerPath == "")
        {
            powerStore.powerPath = findPowerPath();
            if(powerStore.powerPath == "")
            {
                powerHandler(io, powerStore, 1*MICRO_OFFSET);
                return; 
            }
        }

        start = getCurrentTimeWithMs();

        currentPower.time = start*MILLI_OFFSET;
        currentPower.value = readPower(powerStore.powerPath);

        if(powerStore.collectedPower.size() >= MAX_COLLECTION_POWER_SIZE){
            powerStore.collectedPower.erase(powerStore.collectedPower.begin());
            powerStore.collectedPower.push_back(currentPower);
        }
        else{
            powerStore.collectedPower.push_back(currentPower);
        }

        timeoutHandle(currentPower,powerStore);
        
        for (auto it = powerStore.collectedPower.begin(); it != powerStore.collectedPower.end(); it++)
        {
            if(it->time+(powerStore.samplingPeriod*MILLI_OFFSET) < currentPower.time)
            {
                continue;
            }
            if(max < it->value || averageCount == 0)
            {
                max = it->value;
            }
            if(min > it->value || averageCount == 0){
                min = it->value;
            }
            average = average + it->value;
            averageCount++;
        }

        if(averageCount){
            average = average/averageCount;
        }

        updatePowerValue(PERIOD_AVERAGE_PROP,average,powerStore.average);
        updatePowerValue(PERIOD_MAX_PROP,max,powerStore.max);
        updatePowerValue(PERIOD_MIN_PROP,min,powerStore.min);

        end = getCurrentTimeWithMs();
        double delayTime = (1-(end-start))*MICRO_OFFSET;
        if(delayTime < 0){
            delayTime = 0;
        }

        powerHandler(io,powerStore,delayTime);
        return;
    });
}

void timeoutHandle(Power currentPower,PowerStore& powerStore)
{
    if(!powerStore.actionEnable && currentPower.value < powerStore.powerCap)
    {
        powerStore.actionEnable = true;
    }

    if(powerStore.correctionTimeout >= powerStore.correctionTime  && powerStore.powerCapEnable && powerStore.actionEnable)
    {
        static constexpr char const* powerlimitSensorPath= "/xyz/openbmc_project/sensors/power_supply/DCMI_POWER_LIMIT";
        std::vector<uint8_t> powerlimitEventData{0x06,0xFF,0x03};

        powerStore.actionEnable = false;
        if(powerStore.exceptionAction == "xyz.openbmc_project.Control.Power.Cap.Action.None")
        {
            std::cerr << "Action for power limit: None" << "\n";
        }

        if(powerStore.exceptionAction == "xyz.openbmc_project.Control.Power.Cap.Action.OffAndLog")
        {
            std::cerr << "Action for power limit: Power Off and generate SEL event" << "\n";
            generateSELEvent(bus,powerlimitSensorPath,powerlimitEventData);
            hostPowerControl(bus,"xyz.openbmc_project.State.Host.Transition.Off");
        }

        if(powerStore.exceptionAction == "xyz.openbmc_project.Control.Power.Cap.Action.Log")
        {
            std::cerr << "Action for power limit: Generate SEL event" << "\n";
            generateSELEvent(bus,powerlimitSensorPath,powerlimitEventData);
        }

        switch(PowerLimit::convertActionFromString(powerStore.exceptionAction))
        {
            case PowerLimit::Action::None:
                std::cerr << "TEST-1"<< "\n";
                break;
            case PowerLimit::Action::OffAndLog:
                std::cerr << "TEST-2"<< "\n";
                break;
            case PowerLimit::Action::Log:
                std::cerr << "TEST-3"<< "\n";
                break;
            default:
                std::cerr << "TEST-4"<< "\n";
                break;
        }
    }

    if(currentPower.value > powerStore.powerCap &&  powerStore.powerCapEnable)
    {
        powerStore.correctionTimeout = powerStore.correctionTimeout + 1000;
    }
    else
    {
         powerStore.correctionTimeout = 0;
    }
}

inline static sdbusplus::bus::match::match
    startPowerCapMonitor(std::shared_ptr<sdbusplus::asio::connection> conn,PowerStore& powerStore)
{
    auto powerCapMatcherCallback = [&](sdbusplus::message::message &msg)
    {
        std::string interface;
        boost::container::flat_map<std::string, std::variant<std::string, uint16_t, bool, uint32_t>> propertiesChanged;
        msg.read(interface, propertiesChanged);
        std::string event = propertiesChanged.begin()->first;

        if (propertiesChanged.empty() || event.empty())
        {
            return;
        }

        if(event == "SamplingPeriod")
        {
            auto value = std::get_if<uint16_t>(&propertiesChanged.begin()->second);
            powerStore.samplingPeriod = *value;
        }
        if(event == "PowerCapEnable")
        {
            auto value = std::get_if<bool>(&propertiesChanged.begin()->second);
            powerStore.powerCapEnable = *value;
            powerStore.actionEnable = *value;
        }
        if(event == "CorrectionTime")
        {
            auto value = std::get_if<uint32_t>(&propertiesChanged.begin()->second);
            powerStore.correctionTime = *value;
        }
        if(event == "ExceptionAction")
        {
            auto value = std::get_if<std::string>(&propertiesChanged.begin()->second);
            powerStore.exceptionAction = *value;
        }
        if(event == "PowerCap")
        {
            auto value = std::get_if<uint32_t>(&propertiesChanged.begin()->second);
            powerStore.powerCap = *value;
        }

        if constexpr (DEBUG)
        {
            std::cerr << "Properties changed event: " << event <<"\n";
            std::cerr << "PowerStore.samplingPeriod: " <<  powerStore.samplingPeriod <<"\n";
            std::cerr << "PowerStore.powerCapEnable: " <<  powerStore.powerCapEnable <<"\n";
            std::cerr << "PowerStore.correctionTime: " <<  powerStore.correctionTime <<"\n";
            std::cerr << "PowerStore.exceptionAction: " <<  powerStore.exceptionAction <<"\n";
            std::cerr << "PowerStore.powerCap: " <<  powerStore.powerCap <<"\n";
        }

    };

    sdbusplus::bus::match::match powerCapMatcher(
        static_cast<sdbusplus::bus::bus &>(*conn),
        "type='signal',interface='org.freedesktop.DBus.Properties',member='"
        "PropertiesChanged',arg0namespace='xyz.openbmc_project.Control.Power.Cap'",
        std::move(powerCapMatcherCallback));

    return powerCapMatcher;
}

int main(int argc, char *argv[])
{
    boost::asio::io_context io;
    bus = std::make_shared<sdbusplus::asio::connection>(io);
    
    PowerStore powerStore;
    dbusServiceInitialize(powerStore);
    propertyInitialize(powerStore);

    sdbusplus::bus::match::match powerCapMonitor = startPowerCapMonitor(bus,powerStore);

    io.post(
        [&]() { powerHandler(io,powerStore, 0); });
    
    io.run();
    return 0;
}