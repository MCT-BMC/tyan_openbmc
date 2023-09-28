#pragma once

#include <iostream>
#include <fstream>
#include <cstdio>
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>

#include <phosphor-logging/log.hpp>

using namespace phosphor::logging;

constexpr uint8_t baseboardFruBus = 0x02;
constexpr uint8_t baseboardFruAddress = 0x50;

namespace common
{
int getBaseboardFruAddress(uint8_t& bus, uint8_t& addr);
} // namespace common
