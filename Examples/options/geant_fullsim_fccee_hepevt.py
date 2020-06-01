from Gaudi.Configuration import *

###### Parser ################
import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--input', type=str, default=None, help='specify an input File')
my_args, _ = parser.parse_known_args()

from FCCFWCore.joboptions import parse_standard_job_options
args = parse_standard_job_options()


my_input = ""
outfilename = "pgun_output.root"
if args.nevents is not None:
    nEvents = args.nevents

if args.outputfile is not None:
    outfilename = args.outputfile

if my_args.input != None:
    my_input = my_args.input
################################


# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# from Configurables import HepMCReader
from Configurables import HepEVTReader

reader = HepEVTReader("Reader", Filename=my_input)
reader.genparticles.Path="allGenParticles"
reader.genvertices.Path="allGenVertices"

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc, SimG4SingleParticleGeneratorTool
geoservice = GeoSvc("GeoSvc", detectors=[
                                         'Detector/DetFCCeeIDEA/compact/FCCee_DectMaster.xml',
                                         ])

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, FieldComponentZ=0.002, IntegratorStepper="ClassicalRK4")


# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc, SimG4FullSimActions, SimG4UserLimitPhysicsList, SimG4UserLimitRegion
from GaudiKernel.SystemOfUnits import mm

regiontool = SimG4UserLimitRegion("limits", volumeNames=["CDCH"],
                                  maxStep = 2*mm)

physicslisttool = SimG4UserLimitPhysicsList("Physics", fullphysics="SimG4FtfpBert")

actions = SimG4FullSimActions()
actions.enableHistory=True
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist=physicslisttool, 
                        actions=actions, 
                        regions=[regiontool],
                        magneticField=field
                        )

#geantservice.g4PostInitCommands += ["/run/setCut 0.7 mm"]
#geantservice.g4PostInitCommands +=["/process/eLoss/minKinEnergy 1 MeV"]
#geantservice.g4PostInitCommands +=["/score/quantity/energyDeposit edep1kev"]
geantservice.g4PostInitCommands += ["/tracking/storeTrajectory 1"]


# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool, SimG4SaveParticleHistory, SimG4SaveTrajectory
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")


savetrajectorytool = SimG4SaveTrajectory("saveTrajectory")
savetrajectorytool.trajectory.Path = "trajectory"
savetrajectorytool.trajectoryPoints.Path = "trajectoryPoints"


savehisttool = SimG4SaveParticleHistory("saveHistory")
savehisttool.mcParticles.Path = "simParticles"
savehisttool.genVertices.Path = "simVertices"



savetrackertool = SimG4SaveTrackerHits("saveTrackerHits_Barrel", readoutNames = ["VertexBarrelCollection"])
savetrackertool.positionedTrackHits.Path = "positionedHits_barrel"
savetrackertool.trackHits.Path = "hits_barrel"
savetrackertool.digiTrackHits.Path = "digiHits_barrel"

savetrackertool_endcap = SimG4SaveTrackerHits("saveTrackerHits_Endcap", readoutNames = ["VertexEndcapCollection"])
savetrackertool_endcap.positionedTrackHits.Path = "positionedHits_endcap"
savetrackertool_endcap.trackHits.Path = "hits_endcap"
savetrackertool_endcap.digiTrackHits.Path = "digiHits_endcap"

savetrackertool_DCH = SimG4SaveTrackerHits("saveTrackerHits_DCH", readoutNames = ["DriftChamberCollection"])
savetrackertool_DCH.positionedTrackHits.Path = "positionedHits_DCH"
savetrackertool_DCH.trackHits.Path = "hits_DCH"
savetrackertool_DCH.digiTrackHits.Path = "digiHits_DCH"



# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"

geantsim = SimG4Alg("SimG4Alg",
                    outputs= [
        "SimG4SaveTrajectory/saveTrajectory",
        "SimG4SaveParticleHistory/saveHistory",
        "SimG4SaveTrackerHits/saveTrackerHits_Barrel", 
        "SimG4SaveTrackerHits/saveTrackerHits_Endcap",
        "SimG4SaveTrackerHits/saveTrackerHits_DCH"
        ],
                    eventProvider=particle_converter)

from Configurables import MergeDriftChamberHits
mergehits_barrel = MergeDriftChamberHits("MergeHitsBarrel")
mergehits_barrel.DriftChamberHitsPositions.Path = "positionedHits_barrel"
mergehits_barrel.DriftChamberHitsMerged.Path = "trackHits_barrel"
mergehits_barrel.DriftChamberHitsPositionsMerged.Path = "merged_barrel"

mergehits_endcap = MergeDriftChamberHits("MergeHitsEndcap")
mergehits_endcap.DriftChamberHitsPositions.Path = "positionedHits_endcap"
mergehits_endcap.DriftChamberHitsMerged.Path = "trackHits_endcap"
mergehits_endcap.DriftChamberHitsPositionsMerged.Path = "merged_endcap"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                  filename=outfilename,
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, geantsim, mergehits_barrel, mergehits_endcap, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
                )
