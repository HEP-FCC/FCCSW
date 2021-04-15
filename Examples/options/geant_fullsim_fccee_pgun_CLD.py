from Gaudi.Configuration import *
import os

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import ConstPtParticleGun
guntool1 = ConstPtParticleGun("SignalProvider", PdgCodes=[13], PtMin=50, PtMax=50)
from Configurables import GenAlg
gun = GenAlg()
gun.SignalProvider = guntool1
gun.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter()
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.GenParticles.Path = "allGenParticles"



# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc, SimG4SingleParticleGeneratorTool
geoservice = GeoSvc("GeoSvc", detectors=[
                                         os.path.join(os.environ["FCCDETECTORS"], 'Detector/DetFCCeeCLD/compact/FCCee_DectMaster.xml'),
                                         ])

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, FieldComponentZ=0.002, IntegratorStepper="ClassicalRK4")


# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc, SimG4FullSimActions, SimG4UserLimitPhysicsList, SimG4UserLimitRegion
from GaudiKernel.SystemOfUnits import mm

#regiontool = SimG4UserLimitRegion("limits", volumeNames=["world"],
#                                  maxStep = 2*mm)

#physicslisttool = SimG4UserLimitPhysicsList("testlimphys", fullphysics="SimG4FtfpBert")

# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", 
                        #regions=[regiontool],
                        #magneticField=field,
			#g4PostInitCommands=['/random/setSeeds 1234500 3456700']
                        )

#geantservice.g4PostInitCommands += ["/run/setCut 0.7 mm"]
#geantservice.g4PostInitCommands +=["/process/eLoss/minKinEnergy 1 MeV"]
#geantservice.g4PostInitCommands +=["/score/quantity/energyDeposit edep1kev"]
#geantservice.g4PostInitCommands += ["/tracking/storeTrajectory 1"]


# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4SaveCalHits, SimG4PrimariesFromEdmTool, SimG4SaveParticleHistory, SimG4SaveTrajectory
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")




savetrackertool = SimG4SaveTrackerHits("saveTrackerHits_Barrel", readoutNames = ["VertexBarrelCollection"])
savetrackertool.SimTrackHits.Path = "hits_VXD_barrel"

savetrackertool_endcap = SimG4SaveTrackerHits("saveTrackerHits_Endcap", readoutNames = ["VertexEndcapCollection"])
savetrackertool_endcap.SimTrackHits.Path = "hits_VXD_endcap"

savetrackertool_ITB = SimG4SaveTrackerHits("saveTrackerHits_IT_Barrel", readoutNames = ["InnerTrackerBarrelCollection"])
savetrackertool_ITB.SimTrackHits.Path = "hits_IT_barrel"

savetrackertool_ITE = SimG4SaveTrackerHits("saveTrackerHits_IT_Endcap", readoutNames = ["InnerTrackerEndcapCollection"])
savetrackertool_ITE.SimTrackHits.Path = "hits_IT_endcap"

savetrackertool_OTB = SimG4SaveTrackerHits("saveTrackerHits_OT_Barrel", readoutNames = ["OuterTrackerBarrelCollection"])
savetrackertool_OTB.SimTrackHits.Path = "hits_OT_barrel"

savetrackertool_OTE = SimG4SaveTrackerHits("saveTrackerHits_OT_Endcap", readoutNames = ["OuterTrackerEndcapCollection"])
savetrackertool_OTE.SimTrackHits.Path = "hits_OT_endcap"

savelumicaltool = SimG4SaveCalHits("saveLumicalHits", readoutNames = ["LumiCalCollection"])
savelumicaltool.CaloHits.Path = "hits_Lumical"

saveEcalBarreltool = SimG4SaveCalHits("saveEcalBarrelHits", readoutNames = ["ECalBarrelCollection"])
saveEcalBarreltool.CaloHits.Path = "hits_EcalBarrel"

saveEcalEndcaptool = SimG4SaveCalHits("saveEcalEndcapHits", readoutNames = ["ECalEndcapCollection"])
saveEcalEndcaptool.CaloHits.Path = "hits_EcalEndcap"

saveHcalBarreltool = SimG4SaveCalHits("saveHcalBarrelHits", readoutNames = ["HCalBarrelCollection"])
saveHcalBarreltool.CaloHits.Path = "hits_HcalBarrel"

saveHcalEndcaptool = SimG4SaveCalHits("saveHcalEndcapHits", readoutNames = ["HCalEndcapCollection"])
saveHcalEndcaptool.CaloHits.Path = "hits_HcalEndcap"



# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.GenParticles.Path = "allGenParticles"

geantsim = SimG4Alg("SimG4Alg",
                    outputs= [
        #"SimG4SaveTrajectory/saveTrajectory",
        #"SimG4SaveParticleHistory/saveHistory",
        "SimG4SaveTrackerHits/saveTrackerHits_Barrel", 
        "SimG4SaveTrackerHits/saveTrackerHits_Endcap",   #, SimG4SaveTrackerHits/saveTrackerHits_DCH" 
        "SimG4SaveTrackerHits/saveTrackerHits_IT_Barrel",
        "SimG4SaveTrackerHits/saveTrackerHits_IT_Endcap",
        "SimG4SaveTrackerHits/saveTrackerHits_OT_Barrel",
        "SimG4SaveTrackerHits/saveTrackerHits_OT_Endcap",
	      "SimG4SaveCalHits/saveLumicalHits",
  	    "SimG4SaveCalHits/saveEcalBarrelHits",
	      "SimG4SaveCalHits/saveEcalEndcapHits",
        "SimG4SaveCalHits/saveHcalBarrelHits",
        "SimG4SaveCalHits/saveHcalEndcapHits"
        ],
                    eventProvider=particle_converter)


# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                  filename="fccee_cld_out.py",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gun, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
                )
