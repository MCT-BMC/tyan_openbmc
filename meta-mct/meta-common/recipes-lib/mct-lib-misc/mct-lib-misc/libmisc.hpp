#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <nlohmann/json.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>


int getNetworkAddressIPv4(std::string interface ,std::string& ipAddress);

int getPostCodeMap(std::string platform, std::map<int32_t, std::string>& postCodeMap);

std::string getPostCodeDescription(int postCode, std::map<int32_t, std::string>& postCodeMap);
