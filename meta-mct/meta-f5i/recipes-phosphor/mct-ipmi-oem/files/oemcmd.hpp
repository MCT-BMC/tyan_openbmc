#include <stdint.h>
#include <host-ipmid/ipmid-api.h>

struct ManufactureModeRequest
{
       uint8_t manufactureID[3];
       uint8_t mode;
};

struct FanPwmDutyRequest
{
       uint8_t manufactureID[3];
       uint8_t pwmId;
       uint8_t duty;
};


enum ipmi_net_fns_oem
{
    NETFUN_TWITTER_OEM = 0x30,
};

enum ipmi_net_fns_oem_cmds
{
    IPMI_CMD_FanPwmDuty = 0x05,
    IPMI_CMD_ManufactureMode = 0x06,
    IPMI_CMD_ClearCmos = 0x3A,
};


