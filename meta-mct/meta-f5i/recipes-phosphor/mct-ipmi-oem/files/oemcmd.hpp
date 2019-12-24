#include <stdint.h>
#include <host-ipmid/ipmid-api.h>

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


