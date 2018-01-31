################################################################################
# Package: MagneticField
################################################################################

gaudi_subdir(MagneticField v1r0)

gaudi_depends_on_subdirs(GaudiKernel
 						 Math)

find_package(ACTS COMPONENTS Core)
#workaround for the lone -lGeom flag exported by ACTS
link_directories(${ROOT_INCLUDE_DIR}/../lib)

find_package(ROOT COMPONENTS RIO Tree REQUIRED)

gaudi_install_headers(MagneticField)

gaudi_add_module(MagneticField
                 src/components/*.cpp
                 INCLUDE_DIRS GaudiKernel ROOT ACTS Math
                 LINK_LIBRARIES GaudiKernel ROOT ACTS::ACTSCore)
