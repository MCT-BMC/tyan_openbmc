#include "libmisc.hpp"

int getNetworkAddressIPv4(std::string interface ,std::string& ipAddress)
{
    int fd;
    struct ifreq ifr;
    char strIP[INET_ADDRSTRLEN];

    //Create socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        return errno;
    }

    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ-1);
    ifr.ifr_name[IFNAMSIZ-1] = '\0';

    // Get IP address
    if (ioctl(fd, SIOCGIFADDR, &ifr) == -1)
    {
        close(fd);
        return errno;
    }

    close(fd);

    // Convert IP address to string and checking 
    if (inet_ntop(AF_INET, &((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr, strIP, INET_ADDRSTRLEN) == nullptr)
    {
        return errno;
    }

    if (strcmp(strIP, "0.0.0.0") == 0 || strcmp(strIP, "") == 0)
    {
        return errno;
    }

    ipAddress = strIP;
    return 0;
}

int getPostCodeMap(std::string platform, std::map<int32_t, std::string>& postCodeMap)
{
    std::string postCodePath = "/usr/share/mct-post-code-config/" + platform + "-post-code-config.json";
    std::ifstream postCodeConfigJson(postCodePath);

    if(!postCodeConfigJson)
    {
        return -1;
    }

    auto postCodeJson = nlohmann::json::parse(postCodeConfigJson, nullptr, false);

    if(postCodeJson.is_discarded())
    {
        return -1;
    }

    for (const auto& item : postCodeJson)
    {
        postCodeMap[item[0]] = item[1];
    }

    return 0;
}

std::string getPostCodeDescription(int postCode, std::map<int32_t, std::string>& postCodeMap)
{
    std::string postCodeDescription = "Unreconigized post code";
    auto itPostCode = postCodeMap.find(postCode);
    if (itPostCode != postCodeMap.end())
    {
        postCodeDescription = itPostCode->second;
    }

    return postCodeDescription;
}