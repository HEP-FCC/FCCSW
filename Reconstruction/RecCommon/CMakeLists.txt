################################################################################
# Package: RecCommon
################################################################################
gaudi_subdir(RecCommon v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Detector/DetSegmentation Detector/DetCommon Reconstruction/RecInterface)

find_package(FastJet)
find_package(ROOT COMPONENTS Physics)
find_package(FCCEDM)
find_package(PODIO)
find_package(HepMC)
find_package(DD4hep COMPONENTS DDSegmentation)
find_package(Geant4)

gaudi_add_library(RecCommon
                  src/lib/*.cpp
                  INCLUDE_DIRS DD4hep ROOT
                  LINK_LIBRARIES GaudiKernel DD4hep ROOT ${DD4hep_COMPONENT_LIBRARIES}
                  PUBLIC_HEADERS RecCommon)


gaudi_add_module(RecCommonPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS FastJet ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface DetSegmentation Geant4 DetCommon RecInterface RecCommon
                 LINK_LIBRARIES Fastjet ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep DetSegmentation Geant4 DetCommon RecCommon ${DD4hep_COMPONENT_LIBRARIES})

include(CTest)
gaudi_add_test(genJetClustering
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/genJetClustering.py)
