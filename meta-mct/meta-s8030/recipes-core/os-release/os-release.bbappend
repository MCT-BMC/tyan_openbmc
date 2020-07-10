#
# These file add version 

python() {
        d.setVar('VERSION', "v0.01-00-s8030")
        d.setVar('VERSION_ID', "v0.01-00-s8030")
        d.setVar('BMC_NAME', "S8030")
}

OS_RELEASE_FIELDS_append += "BMC_NAME"

# Ensure the git commands run every time bitbake is invoked.
BB_DONT_CACHE = "1"

# Make os-release available to other recipes.
SYSROOT_DIRS_append = " ${sysconfdir}"
