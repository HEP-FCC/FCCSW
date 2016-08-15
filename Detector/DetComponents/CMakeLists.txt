################################################################################
# Package: DetComponents
################################################################################
gaudi_subdir(DetComponents v1r0)

gaudi_depends_on_subdirs(GaudiKernel
                         RecoGeometry
                         FWCore
                         Detector/DetInterface
                         Detector/DetSegmentation
                         Detector/DetExtensions)

find_package(ROOT COMPONENTS MathCore GenVector Geom)

find_package(DD4hep COMPONENTS DDG4 DDSegmentation REQUIRED)

find_package(Geant4)

gaudi_add_module(DetComponents
                 src/*.cpp
                 INCLUDE_DIRS GaudiKernel ROOT DD4hep Geant4 RecoGeometry FWCore DetInterface DetExtensions DetSegmentation
                 LINK_LIBRARIES GaudiKernel ROOT DD4hep ${DD4hep_COMPONENT_LIBRARIES} DetSegmentation Geant4 RecoGeometry FWCore)
