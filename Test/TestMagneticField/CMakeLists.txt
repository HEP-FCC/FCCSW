################################################################################
# Package: TestMagneticField
################################################################################

gaudi_subdir(TestMagneticField v1r0)

gaudi_depends_on_subdirs(GaudiKernel
 						 MagneticField
 						 Sim/SimG4Common
 						 Sim/SimG4Interface)

find_package(Acts COMPONENTS Core)
#workaround for the lone -lGeom flag exported by ACTS
link_directories(${ROOT_INCLUDE_DIR}/../lib)
find_package(Geant4)
include(${Geant4_USE_FILE})

find_package(ROOT COMPONENTS RIO Tree REQUIRED)

gaudi_install_headers(TestMagneticField)

gaudi_add_module(TestMagneticField
                 src/components/*.cpp
                 INCLUDE_DIRS GaudiKernel ROOT Acts MagneticField SimG4Common SimG4Interface Geant4
                 LINK_LIBRARIES GaudiKernel ROOT ActsCore SimG4Common Geant4)
                 
include(CTest)
gaudi_add_test(TestBFieldG4
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/testBFieldG4.py)
               
gaudi_add_test(TestBFieldSymmetry
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/testBFieldSymmetry.py)
               
gaudi_add_test(TestBFieldValue
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/testBFieldValue.py)
               
gaudi_add_test(TestBFieldValueTxt
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               FRAMEWORK tests/options/testBFieldValue_txt.py)