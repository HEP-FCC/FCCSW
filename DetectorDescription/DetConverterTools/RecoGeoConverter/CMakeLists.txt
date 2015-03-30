################################################################################
# Package: RecoGeoConverter
################################################################################
gaudi_subdir(RecoGeoConverter v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         GaudiKernel
			 GaudiSvc
			 DetectorDescription/DetConverterTools/IConverter
			 DetectorDescription/DetDesServices
			 RecoGeometry
			 DetectorDescription/DetExtensions)

find_package(ROOT COMPONENTS MathCore GenVector Geom)

find_package(DD4hep)

gaudi_install_headers(RecoGeoConverter)

gaudi_add_module(RecoGeoConverter
                 src/*.cxx
                 INCLUDE_DIRS ROOT GaudiKernel DD4hep IConverter DetDesServices RecoGeometry DetExtensions
                 LINK_LIBRARIES ROOT GaudiKernel DD4hep RecoGeometry) 

