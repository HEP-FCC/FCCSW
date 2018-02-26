################################################################################
# Package: RecTools
################################################################################
gaudi_subdir(RecTools v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg
						 Reconstruction/RecInterface
						 MagneticField
						 Math
						 Detector/DetInterface)

find_package(ACTS COMPONENTS Core)
#workaround for the lone -lGeom flag exported by ACTS
link_directories(${ROOT_INCLUDE_DIR}/../lib)

gaudi_install_headers(RecTools)

gaudi_add_module(RecTools
                 src/components/*.cpp
                 INCLUDE_DIRS RecInterface Detector/DetInterface MagneticField Math
                 LINK_LIBRARIES GaudiAlgLib ACTS::ACTSCore) 
