################################################################################
# Package: DetComponents
################################################################################
gaudi_subdir(DetComponents v1r0)

gaudi_depends_on_subdirs(GaudiKernel
                         RecoGeometry
                         FWCore
                         Detector/DetInterface
                         Detector/DetSegmentation
                         Detector/DetCommon
                         Detector/DetExtensions)

find_package(ROOT COMPONENTS MathCore GenVector Geom)

find_package(DD4hep COMPONENTS DDG4 DDSegmentation REQUIRED)

find_package(Geant4)

gaudi_add_module(DetComponents
                 src/*.cpp
                 INCLUDE_DIRS GaudiKernel ROOT DD4hep Geant4 RecoGeometry FWCore DetInterface DetExtensions DetSegmentation DetCommon
                 LINK_LIBRARIES GaudiKernel ROOT DD4hep ${DD4hep_COMPONENT_LIBRARIES} DetSegmentation DetCommon Geant4 RecoGeometry FWCore)

include(CTest)
gaudi_add_test(RedoSegmentationXYZ
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               ENVIRONMENT LD_PRELOAD+=build.$BINARY_TAG/lib/libDetSegmentation.so
               FRAMEWORK tests/options/redoSegmentationXYZ.py)
gaudi_add_test(RedoSegmentationPhiEta
               ENVIRONMENT LD_PRELOAD+=build.$BINARY_TAG/lib/libDetSegmentation.so
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/redoSegmentationPhiEta.py)
gaudi_add_test(MergeCells
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               ENVIRONMENT LD_PRELOAD+=build.$BINARY_TAG/lib/libDetSegmentation.so
               FRAMEWORK tests/options/mergeCells.py)
