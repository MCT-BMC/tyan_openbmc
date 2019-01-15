# any existed openbmc components use hard code
# "org.openbmc.HostIpmi" to locate service.
# In order to make compatible, make '--i' optional.
# When specified, kcsbridge will use prefix in dbus name and objpath.
# If not, default dbus name "org.openbmc.HostIpmi" will be used.

SRCREV = "37db8198e9ddc8a3970d1ff0a8b5e7247e5a48e3"

