################################################################################
# Package: RecoGeometry
################################################################################
gaudi_subdir(RecoGeometry v1r0)

gaudi_depends_on_subdirs(Algebra
			 Tracking/TrkEvent/TrkParametersBase
			 Tracking/TrkGeometry/TrkGeometryUtils
			 DetectorDescription/DetExtensions
			 GaudiKernel)

find_package(ROOT COMPONENTS MathCore GenVector Geom)

find_package(DD4hep)

gaudi_add_library(RecoGeometry
                  src/*.cxx
                  PUBLIC_HEADERS RecoGeometry
                  INCLUDE_DIRS Algebra ROOT DD4hep TrkParametersBase TrkGeometryUtils DetExtensions GaudiKernel
                  LINK_LIBRARIES ROOT DD4hep TrkParametersBase GaudiKernel)

