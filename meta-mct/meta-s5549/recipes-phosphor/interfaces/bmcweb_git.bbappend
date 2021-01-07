FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRCREV = "65176d3908b27393d81b72284dbebc47055e517c"

DEPENDS += "boost-url"

SRC_URI += "file://mct_oem_rest.hpp;subdir=git/include \
            file://0001-Support-rest-API-for-MCT-OEM.patch \
            file://0002-Fix-the-issue-for-update-bmcweb-recipe.patch \
            file://0003-Implement-the-SEL-feature-for-redfish-log-service.patch \
            "

# add a user called bmcweb for the server to assume
# bmcweb is part of group shadow for non-root pam authentication
#USERADD_PARAM_${PN} = "-r -s /usr/sbin/nologin -d /home/bmcweb -m -G shadow bmcweb"

#GROUPADD_PARAM_${PN} = "web; redfish "

# Enable CPU Log and Raw PECI support
EXTRA_OECMAKE += "-DBMCWEB_ENABLE_REDFISH_CPU_LOG=ON"
EXTRA_OECMAKE += "-DBMCWEB_ENABLE_REDFISH_RAW_PECI=ON"

# Enable Redfish BMC Journal support
EXTRA_OECMAKE += "-DBMCWEB_ENABLE_REDFISH_BMC_JOURNAL=ON"
EXTRA_OECMAKE += "-DBMCWEB_HTTP_REQ_BODY_LIMIT_MB=128"
EXTRA_OECMAKE += "-DBMCWEB_INSECURE_ENABLE_REDFISH_FW_TFTP_UPDATE=ON"

# Enable MCT OEM Rest support
EXTRA_OECMAKE += "-DBMCWEB_ENABLE_MCT_OEM_REST=ON"