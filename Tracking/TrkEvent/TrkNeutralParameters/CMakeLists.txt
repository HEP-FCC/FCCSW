################################################################################
# Package: TrkNeutralParameters
################################################################################
gaudi_subdir(TrkNeutralParameters v1r0)

gaudi_depends_on_subdirs(Tracking/TrkEvent/TrkParametersBase
			 RecoGeometry)


gaudi_add_library(TrkNeutralParameters
		 src/*.cxx
		 PUBLIC_HEADERS TrkNeutralParameters
	         INCLUDE_DIRS TrkParametersBase RecoGeometry
                 LINK_LIBRARIES TrkParametersBase RecoGeometry)

