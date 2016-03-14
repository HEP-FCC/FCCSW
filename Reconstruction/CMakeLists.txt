################################################################################
# Package: Reconstruction
################################################################################
gaudi_subdir(Reconstruction v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore)

find_package(FastJet)
find_package(ROOT COMPONENTS Physics)
find_package(FCCEDM)
find_package(PODIO)
find_package(HepMC)

gaudi_add_module(Reconstruction
                 src/*.cpp
                 INCLUDE_DIRS FastJet ROOT FWCore HepMC FCCEDM PODIO
                 LINK_LIBRARIES Fastjet ROOT GaudiAlgLib FCCEDM PODIO)


include(CTest)
gaudi_add_test(genJetClustering
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK options/genJetClustering.py)
