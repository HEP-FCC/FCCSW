################################################################################
# Package: TestGeant
################################################################################
gaudi_subdir(TestGeant v1r0)

#this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg GaudiKernel FWCore DataObjects)

find_package(ROOT)
find_package(Geant4)
find_package(ALBERS)
find_package(FCCEDM)

gaudi_add_module(TestGeant
		 src/*.cpp
		 INCLUDE_DIRS ROOT FWCore GaudiKernel Geant4 DataObjects FCCEDM ALBERS
       LINK_LIBRARIES GaudiAlgLib FWCore DataObjects ROOT GaudiKernel Geant4 FCCEDM ALBERS)

