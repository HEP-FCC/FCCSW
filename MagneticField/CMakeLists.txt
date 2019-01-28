################################################################################
# Package: MagneticField
################################################################################

gaudi_subdir(MagneticField v1r0)

gaudi_depends_on_subdirs(GaudiKernel)

find_package(Acts COMPONENTS Core)
#workaround for the lone -lGeom flag exported by Acts
link_directories(${ROOT_INCLUDE_DIR}/../lib)

find_package(ROOT COMPONENTS RIO Tree REQUIRED)

gaudi_install_headers(MagneticField)

gaudi_add_module(MagneticField
                 src/components/*.cpp
                 INCLUDE_DIRS GaudiKernel ROOT Acts
                 LINK_LIBRARIES GaudiKernel ROOT ActsCore)
