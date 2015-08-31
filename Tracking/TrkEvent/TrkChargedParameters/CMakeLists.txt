################################################################################
# Package: TrkChargedParameters
################################################################################
gaudi_subdir(TrkChargedParameters v1r0)

gaudi_depends_on_subdirs(Tracking/TrkEvent/TrkParametersBase
			 RecoGeometry)


gaudi_add_library(TrkChargedParameters
                 src/*.cxx
		 PUBLIC_HEADERS TrkParametersBase
                 INCLUDE_DIRS TrkParametersBase RecoGeometry
                 LINK_LIBRARIES TrkParametersBase RecoGeometry)