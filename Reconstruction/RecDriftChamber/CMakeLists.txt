################################################################################
# Package: RecDriftChamber
################################################################################
gaudi_subdir(RecDriftChamber v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface Detector/DetSegmentation Detector/DetCommon Reconstruction/RecInterface)

find_package(FastJet)
find_package(ROOT COMPONENTS Physics)
find_package(FCCEDM)
find_package(PODIO)
find_package(HepMC)
find_package(DD4hep)
find_package(Geant4)

gaudi_add_module(RecDriftChamberPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS FastJet ROOT FWCore HepMC FCCEDM PODIO DD4hep DetInterface DetSegmentation Geant4 DetCommon RecInterface RecDriftChamber
                 LINK_LIBRARIES Fastjet ROOT GaudiAlgLib FCCEDM PODIO HepMC DD4hep DetSegmentation Geant4 DetCommon ${DD4hep_COMPONENT_LIBRARIES})

include(CTest)


