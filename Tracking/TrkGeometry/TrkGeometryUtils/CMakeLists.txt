################################################################################
# Package: TrkGeometryUtils
################################################################################
gaudi_subdir(TrkGeometryUtils v1r0)

gaudi_depends_on_subdirs(GaudiKernel
			 Algebra)

gaudi_install_headers(TrkGeometryUtils)

find_package(ROOT COMPONENTS MathCore GenVector)

gaudi_add_library(TrkGeometryUtils
                 src/*.cxx
	   	 PUBLIC_HEADERS TrkGeometryUtils
                 INCLUDE_DIRS Algebra ROOT GaudiKernel
		 LINK_LIBRARIES GaudiKernel ROOT)
