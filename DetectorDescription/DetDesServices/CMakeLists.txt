################################################################################
# Package: DetDesServices
################################################################################
gaudi_subdir(DetDesServices v1r0)

gaudi_depends_on_subdirs(GaudiKernel
			 DetectorDescription/DetDesInterfaces
			 DetectorDescription/DetConverterTools/IConverter
			 RecoGeometry)

find_package(DD4hep)

find_package(Geant4)

gaudi_install_headers(DetDesServices)

gaudi_add_module(DetDesServices
		 src/*.cxx
		 INCLUDE_DIRS DetDesInterfaces IConverter DD4hep RecoGeometry Geant4
		 LINK_LIBRARIES GaudiKernel DD4hep Geant4)
