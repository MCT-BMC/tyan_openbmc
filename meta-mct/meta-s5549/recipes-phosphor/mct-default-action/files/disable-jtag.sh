#!/bin/sh
 
JTAG_DEV="1e6e4000.jtag"
JTAG_PATH="/sys/bus/platform/drivers/jtag-aspeed"

echo "Unbind jtag-aspeed jtag driver"
echo -n $JTAG_DEV > $JTAG_PATH/unbind
 
exit 0
