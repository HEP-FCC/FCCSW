################################################################################
# Package: SimG4Common
################################################################################
gaudi_subdir(SimG4Common v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg
						 Math
						 MagneticField)

find_package(Geant4)
include(${Geant4_USE_FILE})
find_package(ROOT)
find_package(FCCEDM)
find_package(PODIO)
find_package(ACTS COMPONENTS Core Examples TGeoPlugin DD4hepPlugin)
#workaround for the lone -lGeom flag exported by ACTS
link_directories(${ROOT_INCLUDE_DIR}/../lib)

gaudi_install_headers(SimG4Common)

gaudi_add_library(SimG4Common
                 src/*.cpp
                 INCLUDE_DIRS FWCore Geant4 ROOT FCCEDM PODIO ACTS Math MagneticField
                 LINK_LIBRARIES GaudiAlgLib Geant4 ROOT  FCCEDM PODIO ACTS::ACTSCore
                 PUBLIC_HEADERS SimG4Common)
