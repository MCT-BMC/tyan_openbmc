#include <stdint.h>
#include <host-ipmid/ipmid-api.h>



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
    IPMI_CMD_ConfigEccLeakyBucket = 0x1A,
    IPMI_CMD_ClearCmos = 0x3A,
    IPMI_CMD_gpioStatus = 0x41,
    IPMI_CMD_PnmGetReading = 0xE2,
};

struct PnmGetReadingRequest
{
    uint8_t type;
    uint8_t reserved1;
    uint8_t reserved2;
};

