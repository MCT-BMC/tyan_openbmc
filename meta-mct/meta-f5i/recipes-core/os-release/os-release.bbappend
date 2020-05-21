#
# These file add version 

python() {
        d.setVar('VERSION', "v0.10-00-s7106")
        d.setVar('VERSION_ID', "v0.10-00-s7106")
        d.setVar('BMC_NAME', "GT62I-B7106")
}

OS_RELEASE_FIELDS_append += "BMC_NAME"

# Ensure the git commands run every time bitbake is invoked.
BB_DONT_CACHE = "1"

# Make os-release available to other recipes.
SYSROOT_DIRS_append = " ${sysconfdir}"
