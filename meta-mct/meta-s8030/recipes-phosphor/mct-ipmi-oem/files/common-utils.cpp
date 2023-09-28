#include "common-utils.hpp"

namespace common
{
int getBaseboardFruAddress(uint8_t& bus, uint8_t& addr)
{
    bus = baseboardFruBus;
    addr =baseboardFruAddress;
    return 0;
}

} // namespace common
