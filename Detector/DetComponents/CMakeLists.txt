################################################################################
# Package: DetComponents
################################################################################
gaudi_subdir(DetComponents v1r0)

gaudi_depends_on_subdirs(GaudiKernel
                         RecoGeometry
                         Detector/DetInterface
                         Detector/DetExtensions)

find_package(ROOT COMPONENTS MathCore GenVector Geom)

find_package(DD4hep COMPONENTS DDG4)

find_package(Geant4)

find_package(ACTS COMPONENTS Core TGeoPlugin DD4hepPlugin)
#workaround for the lone -lGeom flag exported by ACTS
link_directories(${ROOT_INCLUDE_DIR}/../lib)

gaudi_install_headers(DetComponents)

gaudi_add_module(DetComponents
                 src/*.cpp
                 INCLUDE_DIRS GaudiKernel ROOT DD4hep Geant4 RecoGeometry  DetInterfaces DetExtensions ACTS
                 LINK_LIBRARIES GaudiKernel ROOT DD4hep ${DD4hep_DDG4_LIBRARY} Geant4 RecoGeometry ACTS::ACTSCore ACTS::ACTSDD4hepPlugin)
