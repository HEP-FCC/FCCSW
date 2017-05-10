import os

from GaudiKernel.SystemOfUnits import MeV,GeV

#set these in the .sh script                                                                                                                                                                                                                
energy=100*GeV
num_events=1
magnetic_field=0
particleType = "pi-"

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)
# Geant4 service                                                                                                                                                                         
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")

# range cut                                                                                                                                                                                                                        
geantservice.G4commands += ["/run/setCut 0.1 mm"]

# Magnetic field                                                                                                                                                                                                                           
from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",FieldOn=False)

# common ECAL specific information
# readout name
ecalReadoutName = "ECalHitsEta"

# common HCAL specific information
# readout name
hcalReadoutName = "BarHCal_Readout"
# new readout name
newHcalReadoutName = hcalReadoutName + "_phieta"

# Geant4 algorithm                                                                                                                                                                                                       
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools                                                                                                                                          
# and a tool that saves the calorimeter hits                                                                                                                                                                                         
from Configurables import SimG4Alg, SimG4SaveCalHits, InspectHitsCollectionsTool
saveecaltool = SimG4SaveCalHits("saveECalHits", readoutNames = [ecalReadoutName],
                                positionedCaloHits = "ECalPositionedHits",
                                caloHits = "ECalHits")
savehcaltool = SimG4SaveCalHits("saveHCalHits",readoutNames = [hcalReadoutName],
                                positionedCaloHits="HCalPositionedHits",
                                caloHits="HCalHits")

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")                                                                                                                                                     
from Configurables import SimG4SingleParticleGeneratorTool
pgun = SimG4SingleParticleGeneratorTool("SimG4SingleParticleGeneratorTool",saveEdm=True,
                particleName=particleType,energyMin=energy,energyMax=energy,etaMin=0.36,etaMax=0.36,
                OutputLevel =DEBUG)

geantsim = SimG4Alg("SimG4Alg",
                       outputs= ["SimG4SaveCalHits/saveECalHits", "SimG4SaveCalHits/saveHCalHits"],
                       eventProvider=pgun,
                       OutputLevel=DEBUG)

# Configure tools for calo reconstruction                                                                                                                                                                    
from Configurables import CalibrateInLayersTool
calibEcells = CalibrateInLayersTool("Calibrate",
                                    # sampling fraction obtained using SamplingFractionInLayers from DetStudies package
                                    samplingFraction = [0.168] * 4 + [0.176] * 4 + [0.184] * 4 + [0.191] * 4 + [0.198] * 4 + [0.204] * 4 + [0.210] * 4 + [0.215] * 4,
                                    readoutName = ecalReadoutName,
                                    layerFieldName = "cell")

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool
calibHcells = CalibrateCaloHitsTool("CalibrateHCal", invSamplingFraction="31")

from Configurables import CreateCaloCells
createEcells = CreateCaloCells("CreateECaloCells",
                               doCellCalibration=True,
                               calibTool=calibEcells,
                               addCellNoise=False, filterCellNoise=False,
                               OutputLevel=DEBUG,
                               hits="ECalHits",
                               cells="ECalCells")

createHcells = CreateCaloCells("CreateHCaloCells",
                               doCellCalibration=True,
                               calibTool=calibHcells,
                               addCellNoise = False, filterCellNoise = False,
                               OutputLevel = DEBUG,
                               hits="HCalHits",
                               cells="HCalCells")

# additionally for HCal
from Configurables import CreateVolumeCaloPositions
positions = CreateVolumeCaloPositions("positions", OutputLevel = VERBOSE)
positions.hits.Path = "HCalCells"
positions.positionedHits.Path = "HCalPositions"

from Configurables import RedoSegmentation
resegment = RedoSegmentation("ReSegmentation",
                             # old bitfield (readout)
                             oldReadoutName = hcalReadoutName,
                             # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = ["eta","phi"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = newHcalReadoutName,
                             debugPrint = 10,
                             OutputLevel = DEBUG,
                             inhits = "HCalPositions",
                             outhits = "newHCalCells")
# clusters are needed, with deposit position and cellID in bits

positions2 = CreateVolumeCaloPositions("positions2", OutputLevel = VERBOSE)
positions2.hits.Path = "newHCalCells"
positions2.positionedHits.Path = "newHCalPositions"

# Ecal cell positions
positionsEcal = CreateVolumeCaloPositions("positionsEcal", OutputLevel = VERBOSE)
positionsEcal.hits.Path = "ECalCells"
positionsEcal.positionedHits.Path = "ECalPositions"

out = PodioOutput("out", 
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = "output_combCalo_"+str(particleType)+str(int(energy/GeV))+"GeV.root"

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
positions2.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [geantsim,
              createEcells,
              createHcells,
              positions,
              resegment,
              positions2,
              positionsEcal,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = int(num_events),
    ExtSvc = [podioevent, geoservice, geantservice, audsvc],
 )

