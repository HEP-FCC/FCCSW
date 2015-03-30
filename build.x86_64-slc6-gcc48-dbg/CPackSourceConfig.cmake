# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. Example variables are:
#   CPACK_GENERATOR                     - Generator used to create package
#   CPACK_INSTALL_CMAKE_PROJECTS        - For each project (path, name, component)
#   CPACK_CMAKE_GENERATOR               - CMake Generator used for the projects
#   CPACK_INSTALL_COMMANDS              - Extra commands to install components
#   CPACK_INSTALLED_DIRECTORIES           - Extra directories to install
#   CPACK_PACKAGE_DESCRIPTION_FILE      - Description file for the package
#   CPACK_PACKAGE_DESCRIPTION_SUMMARY   - Summary of the package
#   CPACK_PACKAGE_EXECUTABLES           - List of pairs of executables and labels
#   CPACK_PACKAGE_FILE_NAME             - Name of the package generated
#   CPACK_PACKAGE_ICON                  - Icon used for the package
#   CPACK_PACKAGE_INSTALL_DIRECTORY     - Name of directory for the installer
#   CPACK_PACKAGE_NAME                  - Package project name
#   CPACK_PACKAGE_VENDOR                - Package project vendor
#   CPACK_PACKAGE_VERSION               - Package project version
#   CPACK_PACKAGE_VERSION_MAJOR         - Package project version (major)
#   CPACK_PACKAGE_VERSION_MINOR         - Package project version (minor)
#   CPACK_PACKAGE_VERSION_PATCH         - Package project version (patch)

# There are certain generator specific ones

# NSIS Generator:
#   CPACK_PACKAGE_INSTALL_REGISTRY_KEY  - Name of the registry key for the installer
#   CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS - Extra commands used during uninstall
#   CPACK_NSIS_EXTRA_INSTALL_COMMANDS   - Extra commands used during install


SET(CPACK_BINARY_BUNDLE "")
SET(CPACK_BINARY_CYGWIN "")
SET(CPACK_BINARY_DEB "")
SET(CPACK_BINARY_DRAGNDROP "")
SET(CPACK_BINARY_NSIS "")
SET(CPACK_BINARY_OSXX11 "")
SET(CPACK_BINARY_PACKAGEMAKER "")
SET(CPACK_BINARY_RPM "")
SET(CPACK_BINARY_STGZ "")
SET(CPACK_BINARY_TBZ2 "")
SET(CPACK_BINARY_TGZ "")
SET(CPACK_BINARY_TZ "")
SET(CPACK_BINARY_ZIP "")
SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
SET(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
SET(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
SET(CPACK_GENERATOR "TGZ;TBZ2;TZ")
SET(CPACK_IGNORE_FILES "/InstallArea/;/build\\..*/;/\\.svn/;/\\.settings/;\\..*project;\\.gitignore")
SET(CPACK_INSTALLED_DIRECTORIES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW;/")
SET(CPACK_INSTALL_CMAKE_PROJECTS "")
SET(CPACK_INSTALL_PREFIX "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/InstallArea/x86_64-slc6-gcc48-dbg")
SET(CPACK_MODULE_PATH "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/cmake;/afs/cern.ch/user/j/jhrdinka/FCC/GAUDI/GAUDI_v25r2/InstallArea/x86_64-slc6-gcc48-opt/cmake;/afs/cern.ch/user/j/jhrdinka/FCC/GAUDI/GAUDI_v25r2/cmake;/afs/cern.ch/user/j/jhrdinka/FCC/GAUDI/GAUDI_v25r2/cmake/modules;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/cmake/")
SET(CPACK_NSIS_DISPLAY_NAME "FCCSW 999.999.0")
SET(CPACK_NSIS_INSTALLER_ICON_CODE "")
SET(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
SET(CPACK_NSIS_PACKAGE_NAME "FCCSW 999.999.0")
SET(CPACK_OUTPUT_CONFIG_FILE "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/CPackConfig.cmake")
SET(CPACK_PACKAGE_DEFAULT_LOCATION "/")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "/afs/cern.ch/sw/lcg/external/CMake/2.8.6/x86_64-slc6-gcc46-opt/share/cmake-2.8/Templates/CPack.GenericDescription.txt")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FCCSW built using CMake")
SET(CPACK_PACKAGE_FILE_NAME "FCCSW-999.999.0-Source")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "FCCSW 999.999.0")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "FCCSW 999.999.0")
SET(CPACK_PACKAGE_NAME "FCCSW")
SET(CPACK_PACKAGE_RELOCATABLE "true")
SET(CPACK_PACKAGE_VENDOR "Humanity")
SET(CPACK_PACKAGE_VERSION "999.999.0")
SET(CPACK_PACKAGE_VERSION_MAJOR "999")
SET(CPACK_PACKAGE_VERSION_MINOR "999")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_RESOURCE_FILE_LICENSE "/afs/cern.ch/sw/lcg/external/CMake/2.8.6/x86_64-slc6-gcc46-opt/share/cmake-2.8/Templates/CPack.GenericLicense.txt")
SET(CPACK_RESOURCE_FILE_README "/afs/cern.ch/sw/lcg/external/CMake/2.8.6/x86_64-slc6-gcc46-opt/share/cmake-2.8/Templates/CPack.GenericDescription.txt")
SET(CPACK_RESOURCE_FILE_WELCOME "/afs/cern.ch/sw/lcg/external/CMake/2.8.6/x86_64-slc6-gcc46-opt/share/cmake-2.8/Templates/CPack.GenericWelcome.txt")
SET(CPACK_SET_DESTDIR "OFF")
SET(CPACK_SOURCE_CYGWIN "")
SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2;TZ")
SET(CPACK_SOURCE_IGNORE_FILES "/InstallArea/;/build\\..*/;/\\.svn/;/\\.settings/;\\..*project;\\.gitignore")
SET(CPACK_SOURCE_INSTALLED_DIRECTORIES "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW;/")
SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "/afs/cern.ch/user/j/jhrdinka/FCC/FCCSW/build.x86_64-slc6-gcc48-dbg/CPackSourceConfig.cmake")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "FCCSW-999.999.0-Source")
SET(CPACK_SOURCE_TBZ2 "ON")
SET(CPACK_SOURCE_TGZ "ON")
SET(CPACK_SOURCE_TOPLEVEL_TAG "x86_64-slc6-gcc48-dbg-Source")
SET(CPACK_SOURCE_TZ "ON")
SET(CPACK_SOURCE_ZIP "OFF")
SET(CPACK_STRIP_FILES "")
SET(CPACK_SYSTEM_NAME "x86_64-slc6-gcc48-dbg")
SET(CPACK_TOPLEVEL_TAG "x86_64-slc6-gcc48-dbg-Source")
