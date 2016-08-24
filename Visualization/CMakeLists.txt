################################################################################
# Package: Visualization
################################################################################
gaudi_subdir(Visualization v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore)

find_package(FCCEDM)
find_package(DD4hep COMPONENTS DDCore DDG4 DDEve REQUIRED)
find_package(PODIO)

gaudi_install_headers(Visualization)
gaudi_install_python_modules()

gaudi_add_module(Visualization
                 src/*.cpp
                 INCLUDE_DIRS Visualization FWCore FCCEDM PODIO DD4hep 
                 LINK_LIBRARIES  GaudiAlgLib FCCEDM PODIO DD4hep ${DD4hep_LIBRARIES}  ${DD4hep_DDG4_LIBRARY} ${DD4hep_DDEVE_LIBRARY} )

