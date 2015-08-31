################################################################################
# Package: TestTools
################################################################################
gaudi_subdir(TestTools v1r0)

#this declaration will not be needed in the future

gaudi_depends_on_subdirs(GaudiAlg
			 GaudiKernel
			 DataObjects
			 Test/TestInterfaces
			 RecoGeometry)

find_package(ROOT COMPONENTS MathCore)

find_package(ALBERS)
find_package(FCCEDM)

gaudi_add_module(TestTools
		 src/*.cxx
		 INCLUDE_DIRS GaudiAlgLib GaudiKernel DataObjects TestInterfaces Algebra RecoGeometry ROOT ALBERS FCCEDM
                 LINK_LIBRARIES GaudiAlgLib DataObjects GaudiKernel RecoGeometry ROOT ALBERS FCCEDM)

	

