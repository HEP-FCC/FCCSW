################################################################################
# Package: LbPythia8
################################################################################
gaudi_subdir(LbPythia8 v11r1)

gaudi_depends_on_subdirs(Gen/Generators)

find_package(Pythia8 COMPONENTS pythia8 pythia8tohepmc)
find_package(HepMC COMPONENTS fio)

if(EXISTS "${PYTHIA8_INCLUDE_DIR}/Pythia8/Pythia.h")
  # include directory changed from Pythia8 175 to 183
  include_directories(${PYTHIA8_INCLUDE_DIR}/Pythia8)
endif()

gaudi_add_library(LbPythia8Lib
                  src/Lib/*.cpp
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS Pythia8
                  LINK_LIBRARIES GeneratorsLib Pythia8)

gaudi_add_module(LbPythia8
                 src/component/*.cpp
                 LINK_LIBRARIES GeneratorsLib LbPythia8Lib HepMC)

gaudi_env(SET PYTHIA8XML ${PYTHIA8_XML})

