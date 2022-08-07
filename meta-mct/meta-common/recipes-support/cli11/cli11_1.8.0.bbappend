# Fix the git clone error on Ubuntu 20.04
SRC_URI_remove = "gitsm://github.com/CLIUtils/CLI11"
SRC_URI += "gitsm://github.com/CLIUtils/CLI11;branch=main"

