################################################################################
# Package: DetDesServices
################################################################################
gaudi_subdir(DetDesServices v1r0)

gaudi_depends_on_subdirs(GaudiKernel
			 DetectorDescription/DetDesInterfaces)

find_package(DD4hep COMPONENTS DDG4)

find_package(Geant4)


gaudi_install_headers(DetDesServices)

gaudi_add_module(DetDesServices
		 src/*.cxx
		 INCLUDE_DIRS GaudiKernel DD4hep Geant4  DetDesInterfaces 
		 LINK_LIBRARIES GaudiKernel DD4hep ${DD4hep_DDG4_LIBRARY} Geant4)
