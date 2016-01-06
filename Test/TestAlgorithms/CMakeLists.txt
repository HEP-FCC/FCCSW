################################################################################
# Package: TestAlgorithms
################################################################################
gaudi_subdir(TestAlgorithms v1r0)

#this declaration will not be needed in the future

gaudi_depends_on_subdirs(GaudiAlg
			 GaudiKernel
             		 DetectorDescription/DetDesInterfaces
			 Test/TestInterfaces
			 RecoGeometry
                         FWCore
			 DataObjects)

find_package(ROOT COMPONENTS MathCore GenVector Geom)

find_package(DD4hep)

find_package(Geant4)
include(${Geant4_USE_FILE})
find_package(ALBERS)
find_package(FCCEDM)

gaudi_add_module(TestAlgorithms
		 src/*.cxx
		 INCLUDE_DIRS Algebra ROOT  FWCore DetDesInterfaces DD4hep RecoGeometry GaudiKernel Geant4 DataObjects TestInterfaces FCCEDM ALBERS
                 LINK_LIBRARIES GaudiAlgLib FWCore DataObjects ROOT GaudiKernel DD4hep RecoGeometry Geant4 FCCEDM ALBERS)

	

