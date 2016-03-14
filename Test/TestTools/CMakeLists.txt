################################################################################
# Package: TestTools
################################################################################
gaudi_subdir(TestTools v1r0)

#this declaration will not be needed in the future

gaudi_depends_on_subdirs(GaudiAlg
			 GaudiKernel
			 Test/TestInterfaces
			 RecoGeometry)

find_package(ROOT COMPONENTS MathCore)

find_package(PODIO)
find_package(FCCEDM)

gaudi_add_module(TestTools
		 src/*.cxx
		 INCLUDE_DIRS GaudiAlgLib GaudiKernel TestInterfaces Algebra RecoGeometry ROOT PODIO FCCEDM
                 LINK_LIBRARIES GaudiAlgLib GaudiKernel RecoGeometry ROOT PODIO FCCEDM)



