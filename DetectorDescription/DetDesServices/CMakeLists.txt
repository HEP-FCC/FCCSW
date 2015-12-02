################################################################################
# Package: DetDesServices
################################################################################
gaudi_subdir(DetDesServices v1r0)

gaudi_depends_on_subdirs(GaudiKernel
			 RecoGeometry
			 DetectorDescription/DetDesInterfaces
			 DetectorDescription/DetExtensions)

find_package(ROOT COMPONENTS MathCore GenVector Geom)

find_package(DD4hep COMPONENTS DDG4)

find_package(Geant4)

gaudi_install_headers(DetDesServices)

gaudi_add_module(DetDesServices
		 src/*.cxx
		 INCLUDE_DIRS GaudiKernel ROOT DD4hep Geant4 RecoGeometry  DetDesInterfaces DetExtensions
		 LINK_LIBRARIES GaudiKernel ROOT DD4hep ${DD4hep_DDG4_LIBRARY} Geant4 RecoGeometry)
