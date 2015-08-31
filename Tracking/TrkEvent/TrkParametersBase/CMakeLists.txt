################################################################################
# Package: TrkParametersBase
################################################################################
gaudi_subdir(TrkParametersBase v1r0)

gaudi_depends_on_subdirs(GaudiKernel
			 Tracking/TrkGeometry/TrkGeometryUtils)


gaudi_add_library(TrkParametersBase
                 src/*.cxx
		 PUBLIC_HEADERS TrkParametersBase
                 INCLUDE_DIRS TrkGeometryUtils GaudiKernel
                 LINK_LIBRARIES TrkGeometryUtils GaudiKernel)
