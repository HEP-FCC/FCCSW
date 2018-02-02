from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

# ECAL readouts
ecalBarrelReadoutName = "ECalBarrelEta"
ecalBarrelReadoutNamePhiEta = "ECalBarrelPhiEta"
ecalEndcapReadoutName = "EMECPhiEta"
ecalFwdReadoutName = "EMFwdPhiEta"
# HCAL readouts
hcalBarrelReadoutName = "BarHCal_Readout"
hcalBarrelReadoutVolume = "HCalBarrel"
hcalExtBarrelReadoutName = "ExtBarHCal_Readout"
hcalExtBarrelReadoutVolume = "HCalExtBarrel"
hcalEndcapReadoutName = "HECPhiEta"
hcalFwdReadoutName = "HFwdPhiEta"
# Tail Catcher readout
tailCatcherReadoutName = "Muons_Readout"
# Number of events
num_events = 3

podioevent = FCCDataSvc("EventDataSvc", input="output_fullCalo_SimAndDigi_e50GeV_"+str(num_events)+"events.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections = ["ECalBarrelCells", "HCalBarrelCells", "HCalExtBarrelCells", "ECalEndcapCells", "HCalEndcapCells", "ECalFwdCells", "HCalFwdCells", "TailCatcherCells"], OutputLevel = DEBUG)

from Configurables import GeoSvc
detectors_to_use=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                  'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                  'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
                  'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                  'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
                  'file:Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml',
                  'file:Detector/DetFCChhTailCatcher/compact/FCChh_TailCatcher.xml']

geoservice = GeoSvc("GeoSvc", detectors = detectors_to_use, OutputLevel = WARNING)

from Configurables import RewriteBitfield
rewrite = RewriteBitfield("Rewrite",
                          # old bitfield (readout)
                          oldReadoutName = hcalBarrelReadoutName,
                          newReadoutName = hcalBarrelReadoutVolume,
                          # specify if segmentation is removed
                          removeIds = ["tile","eta","phi"],
#                          rmSegmentation = True,
                          # new bitfield (readout), with new segmentation
                          debugPrint = 10,
                          OutputLevel = DEBUG)
# clusters are needed, with deposit position and cellID in bits
rewrite.inhits.Path = "HCalBarrelCells"
rewrite.outhits.Path = "HCalBarrelCellsVol"
rewriteExt = RewriteBitfield("RewriteExt",
                          # old bitfield (readout)
                          oldReadoutName = hcalExtBarrelReadoutName,
                          newReadoutName = hcalExtBarrelReadoutVolume,
                          # specify if segmentation is removed
                          removeIds = ["tile","eta","phi"],
#                          rmSegmentation = True,
                          # new bitfield (readout), with new segmentation
                          debugPrint = 10,
                          OutputLevel = DEBUG)
# clusters are needed, with deposit position and cellID in bits
rewriteExt.inhits.Path = "HCalExtBarrelCells"
rewriteExt.outhits.Path = "HCalExtBarrelCellsVol"

# Merge cells according to CellId
from Configurables import CreateCaloCells
createNewHcells = CreateCaloCells("CreateNewHCaloCells",
                               doCellCalibration=False,
                               addCellNoise=False, filterCellNoise=False,
                               OutputLevel=DEBUG,
                               hits="HCalBarrelCellsVol",
                               cells="HCalBarrelCellsForTopo")
createNewExtHcells = CreateCaloCells("CreateNewExtHCaloCells",
                               doCellCalibration=False,
                               addCellNoise=False, filterCellNoise=False,
                               OutputLevel=DEBUG,
                               hits="HCalExtBarrelCellsVol",
                               cells="HCalExtBarrelCellsForTopo")

#Configure tools for calo cell positions
from Configurables import CellPositionsECalBarrelTool, CellPositionsHCalBarrelTool, CellPositionsHCalBarrelNoSegTool, CellPositionsCaloDiscsTool, CellPositionsTailCatcherTool 
ECalBcells = CellPositionsECalBarrelTool("CellPositionsECalBarrel", 
                                         readoutName = ecalBarrelReadoutNamePhiEta, 
                                         OutputLevel = INFO)
EMECcells = CellPositionsCaloDiscsTool("CellPositionsEMEC", 
                                       readoutName = ecalEndcapReadoutName, 
                                       OutputLevel = INFO)
ECalFwdcells = CellPositionsCaloDiscsTool("CellPositionsECalFwd", 
                                          readoutName = ecalFwdReadoutName, 
                                          OutputLevel = INFO)
HCalBcells = CellPositionsHCalBarrelTool("CellPositionsHCalBarrel", 
                                         readoutName = hcalBarrelReadoutName, 
                                         OutputLevel = INFO)
HCalExtBcells = CellPositionsHCalBarrelTool("CellPositionsHCalExtBarrel", 
                                            readoutName = hcalExtBarrelReadoutName, 
                                            OutputLevel = INFO)
HCalBcellVols = CellPositionsHCalBarrelNoSegTool("CellPositionsHCalBarrelVols", 
                                                 readoutName = hcalBarrelReadoutVolume, 
                                                 OutputLevel = INFO)
HCalExtBcellVols = CellPositionsHCalBarrelNoSegTool("CellPositionsHCalExtBarrelVols", 
                                                    readoutName = hcalExtBarrelReadoutVolume, 
                                                    OutputLevel = INFO)
HECcells = CellPositionsCaloDiscsTool("CellPositionsHEC", 
                                      readoutName = hcalEndcapReadoutName, 
                                      OutputLevel = INFO)
HCalFwdcells = CellPositionsCaloDiscsTool("CellPositionsHCalFwd", 
                                          readoutName = hcalFwdReadoutName, 
                                          OutputLevel = INFO)
TailCatchercells = CellPositionsTailCatcherTool("CellPositionsTailCatcher", 
                                                readoutName = tailCatcherReadoutName, 
                                                centralRadius = 901.5,
                                                OutputLevel = INFO)

# cell positions
from Configurables import CreateCellPositions
positionsEcalBarrel = CreateCellPositions("positionsEcalBarrel", 
                                          positionsTool=ECalBcells, 
                                          hits = "ECalBarrelCells", 
                                          positionedHits = "ECalBarrelCellPositions", 
                                          OutputLevel = INFO)
positionsHcalBarrel = CreateCellPositions("positionsHcalBarrel", 
                                          positionsTool=HCalBcells, 
                                          hits = "HCalBarrelCells", 
                                          positionedHits = "HCalBarrelCellPositions", 
                                          OutputLevel = INFO)
positionsHcalExtBarrel = CreateCellPositions("positionsHcalExtBarrel", 
                                          positionsTool=HCalExtBcells, 
                                          hits = "HCalExtBarrelCells", 
                                          positionedHits = "HCalExtBarrelCellPositions", 
                                          OutputLevel = INFO)
positionsHcalBarrelVol = CreateCellPositions("positionsHcalBarrelVol", 
                                          positionsTool=HCalBcellVols, 
                                          hits = "HCalBarrelCellsForTopo", 
                                          positionedHits = "HCalBarrelCellForTopoPositions", 
                                          OutputLevel = INFO)
positionsHcalExtBarrelVol = CreateCellPositions("positionsHcalExtBarrelVol", 
                                          positionsTool=HCalExtBcellVols, 
                                          hits = "HCalExtBarrelCellsForTopo", 
                                          positionedHits = "HCalExtBarrelCellForTopoPositions", 
                                          OutputLevel = INFO)
positionsEcalEndcap = CreateCellPositions("positionsEcalEndcap", 
                                          positionsTool=EMECcells, 
                                          hits = "ECalEndcapCells", 
                                          positionedHits = "ECalEndcapCellPositions", 
                                          OutputLevel = INFO)
positionsHcalEndcap = CreateCellPositions("positionsHcalEndcap", 
                                          positionsTool=HECcells, 
                                          hits = "HCalEndcapCells", 
                                          positionedHits = "HCalEndcapCellPositions", 
                                          OutputLevel = INFO)
positionsEcalFwd = CreateCellPositions("positionsEcalFwd", 
                                          positionsTool=ECalFwdcells, 
                                          hits = "ECalFwdCells", 
                                          positionedHits = "ECalFwdCellPositions", 
                                          OutputLevel = INFO)
positionsHcalFwd = CreateCellPositions("positionsHcalFwd", 
                                          positionsTool=HCalFwdcells, 
                                          hits = "HCalFwdCells", 
                                          positionedHits = "HCalFwdCellPositions", 
                                          OutputLevel = INFO)
positionsTailCatcher = CreateCellPositions("positionsTailCatcher", 
                                          positionsTool=TailCatchercells, 
                                          hits = "TailCatcherCells", 
                                          positionedHits = "TailCatcherCellPositions", 
                                          OutputLevel = INFO)

out = PodioOutput("out", OutputLevel=DEBUG)
out.filename = "~/FCCSW/digi_cellPostions_pim_100GeV.root"
out.outputCommands = ["keep *","drop ECalBarrelCells","drop ECalEndcapCells","drop ECalFwdCells","drop HCalBarrelCellsVol","drop HCalBarrelCellsForTopo","drop HCalBarrelCells","drop HCalExtBarrelCellsForTopo", "drop HCalExtBarrelCellsVol", "drop HCalExtBarrelCells", "drop HCalEndcapCells", "drop HCalFwdCells", "drop TailCatcherCells"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
rewrite.AuditExecute = True
rewriteExt.AuditExecute = True
createNewHcells.AuditExecute = True
createNewExtHcells.AuditExecute = True
positionsEcalBarrel.AuditExecute = True
positionsEcalEndcap.AuditExecute = True
positionsEcalFwd.AuditExecute = True
positionsHcalBarrel.AuditExecute = True
positionsHcalExtBarrel.AuditExecute = True
positionsHcalBarrelVol.AuditExecute = True
positionsHcalExtBarrelVol.AuditExecute = True
positionsHcalEndcap.AuditExecute = True
positionsHcalFwd.AuditExecute = True
positionsTailCatcher.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
TopAlg = [    podioinput,
              rewrite,
              rewriteExt,
              createNewHcells,
              createNewExtHcells,
              positionsEcalBarrel,
              positionsEcalEndcap,
              positionsEcalFwd, 
              positionsHcalBarrel, 
              positionsHcalExtBarrel,
              positionsHcalBarrelVol,
              positionsHcalExtBarrelVol,
              positionsHcalEndcap, 
              positionsHcalFwd,
              positionsTailCatcher,
              out
              ],
    EvtSel = 'NONE',
    EvtMax = 1,
    ExtSvc = [geoservice, podioevent, audsvc],
)
