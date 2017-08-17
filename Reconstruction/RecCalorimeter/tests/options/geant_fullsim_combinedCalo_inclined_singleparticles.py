import os
import numpy as np

#loads array of random seeds from file                                                                                                         
seed_array = np.loadtxt('condor/seeds.txt',dtype='int',delimiter=',')

#set these in the .sh script                                                                                                                   
energy=100
num_events=10
magnetic_field=1
i=1
particle=0

particleType = "pi-"
if particle==0:
    particleType = "e-"
if particle==2:
    particleType = "mu-"
print particleType

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)

# Geant4 service                   
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")

#Setting random seeds for Geant4 simulations
geantservice.g4PreInitCommands += ["/random/setSeeds "+str(seed_array[i-1])+" 0"]
#since the loop to generate the subjobs begins with 1, we need (i-1) to index

# range cut                                                                                    
geantservice.g4PreInitCommands += ["/run/setCut 0.1 mm"]

# Magnetic field                                                                                             
from Configurables import SimG4ConstantMagneticFieldTool
if magnetic_field==1:
    field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",FieldOn=True,IntegratorStepper="ClassicalRK4")
else:
    field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",FieldOn=False)

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools                                   
# and a tool that saves the calorimeter hits

from Configurables import SimG4Alg, SimG4SaveCalHits
saveecaltool = SimG4SaveCalHits("saveECalHits", readoutNames = ["ECalBarrelEta"],
                                positionedCaloHits = "ECalPositionedHits",
                                caloHits = "ECalHits")
savehcaltool = SimG4SaveCalHits("saveHCalHits",readoutNames = ["BarHCal_Readout"],
                                positionedCaloHits="HCalPositionedHits",
                                caloHits="HCalHits")

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
from Configurables import SimG4SingleParticleGeneratorTool
pgun = SimG4SingleParticleGeneratorTool("SimG4SingleParticleGeneratorTool",saveEdm=True,
                particleName=particleType,energyMin=energy*1000,energyMax=energy*1000,etaMin=0.0,etaMax=0.0,
                OutputLevel =DEBUG)

geantsim = SimG4Alg("SimG4Alg",
                       outputs= ["SimG4SaveCalHits/saveECalHits", "SimG4SaveCalHits/saveHCalHits"],
                       eventProvider=pgun,
                       OutputLevel=INFO)

# common ECAL specific information
# readout name
ecalReadoutName = "ECalBarrelEta"
# common HCAL specific information
# readout name
hcalReadoutName = "BarHCal_Readout"
# readout name
newHcalReadoutName = hcalReadoutName + "_phieta"

# Configure tools for calo reconstruction                                       
#from Configurables import CalibrateInLayersTool
#calibEcells = CalibrateInLayersTool("Calibrate",
                                    # sampling fraction obtained using SamplingFractionInLayers from DetStudies package
#                                    samplingFraction = [0.152422337199] * 4 + [0.171224643325] * 18 + [0.17584501478] * 18 + [0.18025181064] * 18 + [0.186098100471] * 18 + [0.190591655433] * 18 + [0.193360990074] * 18 + [0.191570316922] * 18,
#                                    readoutName = ecalReadoutName,
#                                    layerFieldName = "layer")

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool
calibHcells = CalibrateCaloHitsTool("CalibrateHCal", invSamplingFraction="31")

from Configurables import CreateCaloCells
createEcells = CreateCaloCells("CreateECaloCells",
                               doCellCalibration=False,
#                               calibTool=calibEcells,
                               addCellNoise=False, filterCellNoise=False,
                               OutputLevel=INFO,
                               hits="ECalHits",
                               cells="ECalCells")

createHcells = CreateCaloCells("CreateHCaloCells",
                               doCellCalibration=True,
                               calibTool=calibHcells,
                               addCellNoise = False, filterCellNoise = False,
                               OutputLevel = INFO,
                               hits="HCalHits",
                               cells="HCalCells")

# additionally for HCal
from Configurables import CreateVolumeCaloPositions
positions = CreateVolumeCaloPositions("positions", OutputLevel = INFO)
positions.hits.Path = "HCalCells"
positions.positionedHits.Path = "HCalPositions"

from Configurables import RedoSegmentation
resegment = RedoSegmentation("ReSegmentation",
                             # old bitfield (readout)
                             oldReadoutName = hcalReadoutName,
                             # # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = ["eta","phi"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = newHcalReadoutName,
                             debugPrint = 10,
                             OutputLevel = INFO,
                             inhits = "HCalPositions",
                             outhits = "newHCalCells")
# clusters are needed, with deposit position and cellID in bits

# Ecal cell positions
positionsEcal = CreateVolumeCaloPositions("positionsEcal", OutputLevel = INFO)
positionsEcal.hits.Path = "ECalCells"
positionsEcal.positionedHits.Path = "ECalPositions"

out = PodioOutput("out", 
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *", "drop ECalHits", "drop ECalPositionedHits", "drop HCalHits", "drop HCalPositionedHits", "drop HCalCells", "drop HCalPositions", "drop ECalCells"]
#out.outputCommands = ["keep *", "drop ECalHits", "drop ECalPositionedHits", "drop ECalCells"]    
out.filename = "output.root"

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True
createEcells.AuditExecute = True
createHcells.AuditExecute = True
positions.AuditExecute = True
resegment.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [geantsim,
              createEcells,
              createHcells,
              positions,
              resegment,
              positionsEcal,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = int(num_events),
    ExtSvc = [geoservice, podioevent, geantservice, audsvc],
 )

