#include <stdint.h>
#include <host-ipmid/ipmid-api.h>

static const size_t retPostCodeLen = 20;

enum ipmi_net_fns_oem
{
    NETFUN_TWITTER_OEM = 0x30,
};

#define IANA_TYAN 0x0019fd
enum ipmi_net_fns_oem_cmds
{
    IPMI_CMD_FanPwmDuty = 0x05,
    IPMI_CMD_ManufactureMode = 0x06,
    IPMI_CMD_FloorDuty = 0x07,
    IPMI_CMD_SetFruField = 0x0B,
    IPMI_CMD_GetFruField = 0x0C,
    IPMI_CMD_SetService = 0x0D,
    IPMI_CMD_GetService = 0x0E,
    IPMI_CMD_GetFirmwareString = 0x10,
    IPMI_CMD_ConfigEccLeakyBucket = 0x1A,
    IPMI_CMD_ClearCmos = 0x3A,
    IPMI_CMD_gpioStatus = 0x41,
    IPMI_CMD_PnmGetReading = 0xE2,
    IPMI_CMD_RamdomDelayACRestorePowerON = 0x18,
    IPMI_CMD_GetPostCode = 0x10,
    IPMI_CMD_RelinkLan = 0x12,
};

struct PnmGetReadingRequest
{
    uint8_t type;
    uint8_t reserved1;
    uint8_t reserved2;
};
