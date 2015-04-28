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
			 DataObjects)

find_package(ROOT COMPONENTS MathCore GenVector Geom)

find_package(DD4hep)

find_package(Geant4)

gaudi_add_module(TestAlgorithms
		 src/*.cxx
		 INCLUDE_DIRS Algebra ROOT  DetDesInterfaces DD4hep RecoGeometry GaudiKernel Geant4 DataObjects TestInterfaces
                 LINK_LIBRARIES GaudiAlgLib DataObjects ROOT GaudiKernel DD4hep RecoGeometry Geant4)

	

