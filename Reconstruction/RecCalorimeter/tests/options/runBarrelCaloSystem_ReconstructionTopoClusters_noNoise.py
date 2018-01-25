import os
#import numpy as np
from GaudiKernel.SystemOfUnits import MeV,GeV
# ECAL readouts
ecalBarrelReadoutName = "ECalBarrelEta"
ecalBarrelReadoutNamePhiEta = "ECalBarrelPhiEta"
ecalEndcapReadoutName = "EMECPhiEta"
ecalFwdReadoutName = "EMFwdPhiEta"
# HCAL readouts
hcalBarrelReadoutName = "BarHCal_Readout"
hcalBarrelReadoutNamePhiEta = hcalBarrelReadoutName + "_phieta"
hcalExtBarrelReadoutName = "ExtBarHCal_Readout"
hcalExtBarrelReadoutNamePhiEta = hcalExtBarrelReadoutName + "_phieta"
hcalEndcapReadoutName = "HECPhiEta"
hcalFwdReadoutName = "HFwdPhiEta"
# Tail Catcher readout
tailCatcherReadoutName = "Muons_Readout"
# layer radii needed for cell positions after re-segmentation 
ecalBarrelLayerRadii = [193.0] + [198.5] + [207.5] + [216.5] + [225.5] + [234.5] + [243.5] + [252.5];
# layers to be merged in endcaps, field name of the readout
ecalEndcapNumberOfLayersToMerge = [2] + [2] + [4]*38
hcalEndcapNumberOfLayersToMerge = [2] + [4]*20
identifierName = "layer"
volumeName = "layer"

num_events=3

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input="output_fullCalo_SimAndDigi_e50GeV_"+str(num_events)+"events.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections = ["ECalBarrelCells", "HCalBarrelCells"], OutputLevel = DEBUG)

from Configurables import GeoSvc
detectors_to_use=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                  'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                  'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
                  'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                  'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
                  'file:Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml',
                  'file:Detector/DetFCChhTailCatcher/compact/FCChh_TailCatcher.xml',
                  'file:Detector/DetFCChhBaseline1/compact/FCChh_Solenoids.xml',
                  'file:Detector/DetFCChhBaseline1/compact/FCChh_Shielding.xml']

geoservice = GeoSvc("GeoSvc", detectors = detectors_to_use, OutputLevel = INFO)

#Configure tools for calo reconstruction
from Configurables import ReadNoiseFromFileTool
noiseToolECal = ReadNoiseFromFileTool("ReadNoiseFromFileToolECal",
                                      readoutName = ecalBarrelReadoutNamePhiEta,
                                      noiseFileName = "/afs/cern.ch/user/n/novaj/public/elecNoise_sfcorrection_50Ohm_default_differentTraces_168.root",
                                      elecNoiseHistoName = "h_elecNoise_fcc_",
                                      activeFieldName = "layer",
                                      addPileup = False,
                                      numRadialLayers = 8)
#Configure tools for calo cell positions
from Configurables import CellPositionsECalBarrelTool, CellPositionsHCalBarrelTool, CellPositionsCaloDiscsTool, CellPositionsTailCatcherTool 
ECalBcells = CellPositionsECalBarrelTool("CellPositionsECalBarrel", 
                                         readoutName = ecalBarrelReadoutNamePhiEta, 
                                         layerRadii = ecalBarrelLayerRadii, 
                                         OutputLevel = INFO)
HCalBcells = CellPositionsHCalBarrelTool("CellPositionsHCalBarrel", 
                                         readoutName = hcalBarrelReadoutName, 
                                         OutputLevel = INFO)
HCalBcellsPhiEta = CellPositionsHCalBarrelTool("CellPositionsHCalBarrelPhiEta", 
                                               readoutName = hcalBarrelReadoutNamePhiEta, 
                                               OutputLevel = INFO)


# resegment HCAL
# Create Cell Positions Collection
from Configurables import CreateCellPositions
positionsHcalBarrel = CreateCellPositions("positionsHcalBarrel", 
                                          positionsTool=HCalBcells, 
                                          hits = "HCalBarrelCells", 
                                          positionedHits = "HCalBarrelCellPositions", 
                                          OutputLevel = INFO)
# Resegment the cellIDs
from Configurables import RedoSegmentation
resegmentHcal = RedoSegmentation("ReSegmentationHcal",
                                 # old bitfield (readout)
                                 oldReadoutName = hcalBarrelReadoutName,
                                 # # specify which fields are going to be altered (deleted/rewritten)
                                 oldSegmentationIds = ["row","tile","eta"],
                                 # new bitfield (readout), with new segmentation
                                 newReadoutName = hcalBarrelReadoutNamePhiEta,
                                 OutputLevel = INFO,
                                 inhits = "HCalBarrelCellPositions",
                                 outhits = "newHCalBarrelCells")
# Merge cells according to CellId
from Configurables import CreateCaloCells
createNewHcells = CreateCaloCells("CreateNewHCaloCells",
                               doCellCalibration=False,
                               addCellNoise=False, filterCellNoise=False,
                               OutputLevel=INFO,
                               hits="newHCalBarrelCells",
                               cells="HCalBarrelCellsAfterResegmentation")

#Create topo clusters
from Configurables import CombinedCaloTopoCluster
createTopoClusters = CombinedCaloTopoCluster("CreateTopoClusters",
                                             ECalCells = "ECalBarrelCells",
                                             HCalCells = "HCalBarrelCells",
                                             ECalReadoutName = ecalBarrelReadoutNamePhiEta,
                                             HCalReadoutName = hcalBarrelReadoutName,
                                             fieldsForNeighboursECal = ["layer","eta","phi"],
                                             fieldsForNeighboursHCal = ["layer","row","phi"],
                                             noiseAddedECal = False,
                                             noiseAddedHCal = False,
#                                             noiseToolECal = noiseToolECal,
#                                             noiseToolHCal = noiseToolHCal,
                                             positionsToolECal = ECalBcells,
                                             positionsToolHCal = HCalBcells,
                                             neighboursRange = 2,
                                             lastECalLayer = 7,
                                             OutputLevel = INFO)
createTopoClusters.clusters.Path = "caloClustersBarrel"
createTopoClusters.clusterCellsECal.Path = "caloClusterBarrelECalCells"
createTopoClusters.clusterCellsHCal.Path = "caloClusterBarrelHCalCells"

out = PodioOutput("out", filename = "output_testTopoClusters_e50GeV_ev1.root",
                  OutputLevel=DEBUG)
out.outputCommands = ["drop *", "keep caloClustersBarrel", "keep caloClusterBarreECallCells", "keep caloClusterBarreHCallCells"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
positionsHcalBarrel.AuditExecute = True
resegmentHcal.AuditExecute = True
createNewHcells.AuditExecute = True
createTopoClusters.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              positionsHcalBarrel,
              resegmentHcal,
              createNewHcells,
              createTopoClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 3,
    ExtSvc = [geoservice, podioevent, audsvc],
)
