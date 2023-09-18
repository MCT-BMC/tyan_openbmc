#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <string>
#include <gpiod.h>
#include <fcntl.h>
#include <sdbusplus/bus.hpp>
#include <boost/algorithm/string/predicate.hpp>

constexpr auto DBUS_PROPERTY_INTERFACE = "org.freedesktop.DBus.Properties";

constexpr auto POWER_SERVICE = "xyz.openbmc_project.State.Host";
constexpr auto POWER_PATH = "/xyz/openbmc_project/state/host0";
constexpr auto POWER_INTERFACE = "xyz.openbmc_project.State.Host";
constexpr auto POWER_PROPERTY = "RequestedHostTransition";

constexpr auto IPMB_SERVICE = "xyz.openbmc_project.Ipmi.Channel.Ipmb";
constexpr auto IPMB_PATH = "/xyz/openbmc_project/Ipmi/Channel/Ipmb";
constexpr auto IPMB_INTERFACE = "org.openbmc.Ipmb";

constexpr auto SPI_DEV="1e630000.spi";
constexpr auto SPI_PATH="/sys/bus/platform/drivers/aspeed-smc/";

constexpr auto BIND=1;
constexpr auto UNBIND=0;

auto bus = sdbusplus::bus::new_default_system();

int initiateStateTransition(std::string power)
{
    std::variant<std::string> state("xyz.openbmc_project.State.Host.Transition." + power);
    try
    {
        auto method = bus.new_method_call(POWER_SERVICE, POWER_PATH,
                                          DBUS_PROPERTY_INTERFACE, "Set");
        method.append(POWER_INTERFACE, POWER_PROPERTY , state);

        bus.call_noreply(method);
    }
    catch (const std::exception&  e)
    {
        std::cerr << "Error in " << __func__ << std::endl;
        return -1;
    }
     return 0;
}

bool checkHostPower()
{
    std::variant<std::string> state;

    auto method = bus.new_method_call(POWER_SERVICE, POWER_PATH,
                                      DBUS_PROPERTY_INTERFACE, "Get");
    method.append(POWER_INTERFACE, "CurrentHostState");
    try
    {
        auto reply = bus.call(method);
        reply.read(state);
    }
    catch (const std::exception&  e)
    {
        std::cerr << "Error in " << __func__ << std::endl;
        return false;
    }

    return boost::ends_with(std::get<std::string>(state), "Running");
}

int setMeToRecoveryMode()
{
    uint8_t commandAddress = 1;
    uint8_t netfn = 0x2e;
    uint8_t lun = 0;
    uint8_t command = 0xdf;

    std::vector<uint8_t> commandData = {0x57, 0x01, 0x00, 0x1};

    auto method = bus.new_method_call(IPMB_SERVICE, IPMB_PATH,
                                      IPMB_INTERFACE, "sendRequest");

    method.append(commandAddress, netfn, lun, command, commandData);
    try
    {
        bus.call_noreply(method);
    }
    catch (const std::exception&  e)
    {
        std::cerr << "Error in " << __func__ << std::endl;
        return -1;
    }

    return 0;
}

int setMeReset()
{
    uint8_t commandAddress = 1;
    uint8_t netfn = 0x6;
    uint8_t lun = 0;
    uint8_t command = 0x02;

    std::vector<uint8_t> commandData = {};

    auto method = bus.new_method_call(IPMB_SERVICE, IPMB_PATH,
                                      IPMB_INTERFACE, "sendRequest");

    method.append(commandAddress, netfn, lun, command, commandData);
    try
    {
        bus.call_noreply(method);
    }
    catch (const std::exception&  e)
    {
        std::cerr << "Error in " << __func__ << std::endl;
        return -1;
    }

    return 0;
}

void setOutput(std::string lineName,int setValue)
{
    int value;
    struct gpiod_line *line = nullptr;;

    line = gpiod_line_find(lineName.c_str());
    if (gpiod_line_request_output(line,"set-output",setValue) < 0)
    {
        std::cerr << "Error request line" << std::endl;;
    }
    gpiod_line_release(line);
}

int setBiosMtdDevice(uint8_t state)
{
    std::string spi = SPI_DEV;
    std::string path = SPI_PATH;
    int fd;

    switch (state)
    {
    case UNBIND:
        path = path + "unbind";
        break;
    case BIND:
        path = path + "bind";
        break;
    default:
        std::cerr << "Fail to get state failed" << std::endl;
        return -1;
    }

    fd = open(path.c_str(), O_WRONLY);
    if (fd < 0)
    {
        std::cerr << "Fail in " << __func__ << std::endl;
        return -1;
    }

    write(fd, spi.c_str(), spi.size());
    close(fd);

    return 0;
}