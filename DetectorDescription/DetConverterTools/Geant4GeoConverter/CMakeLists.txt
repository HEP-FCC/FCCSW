################################################################################
# Package: Geant4GeoConverter
################################################################################
gaudi_subdir(Geant4GeoConverter v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         GaudiKernel
			 DetectorDescription/DetConverterTools/IConverter
			 DetectorDescription/DetDesServices)

find_package(DD4hep)

find_package(Geant4)

gaudi_install_headers(Geant4GeoConverter)

gaudi_add_module(Geant4GeoConverter
                  src/*.cxx
                  INCLUDE_DIRS GaudiKernel DD4hep IConverter DetectorDescription/DetDesServices Geant4
                  LINK_LIBRARIES GaudiKernel DD4hep Geant4)