################################################################################
# Package: RecTracker
################################################################################
gaudi_subdir(RecTracker v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface)

find_package(FCCEDM)
find_package(PODIO)
find_package(HepMC)
find_package(DD4hep COMPONENTS DDG4 DDSegmentation DDRec REQUIRED)
find_package(ROOT)
find_package(ACTS COMPONENTS Core Examples TGeoPlugin DD4hepPlugin)
#workaround for the lone -lGeom flag exported by ACTS
link_directories(${ROOT_INCLUDE_DIR}/../lib)

gaudi_add_module(RecTracker
                 src/components/*.cpp
                 INCLUDE_DIRS ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface RecInterface ACTS
                 LINK_LIBRARIES ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep ${DD4hep_COMPONENT_LIBRARIES} ACTS::ACTSCore ACTS::ACTSDD4hepPlugin ACTS::ACTSExamples) 

include(CTest)
gaudi_add_test(trackFitExample
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/trackFitExample.py)
