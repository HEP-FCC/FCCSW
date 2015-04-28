################################################################################
# Package: TestTools
################################################################################
gaudi_subdir(TestTools v1r0)

#this declaration will not be needed in the future

gaudi_depends_on_subdirs(GaudiAlg
			 GaudiKernel
			 DataObjects
			 Test/TestInterfaces)

gaudi_add_module(TestTools
		 src/*.cxx
		 INCLUDE_DIRS GaudiAlgLib GaudiKernel DataObjects TestInterfaces
                 LINK_LIBRARIES GaudiAlgLib DataObjects GaudiKernel)

	

