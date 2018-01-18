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

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input="/eos/experiment/fcc/hh/simulation/samples/v01/singlePart/pim/bFieldOn/eta0/100GeV/simu/output_condor_novaj_201801061005105814.root")
#/eos/experiment/fcc/hh/simulation/samples/v01/physics/ljets/bFieldOn/etaTo1.5/100GeV/simu/output_lsf_helsens_201801080501028391.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections = ["ECalBarrelCells", "HCalBarrelCells", "HCalExtBarrelCells", "ECalEndcapCells", "HCalEndcapCells", "ECalFwdCells", "HCalFwdCells", "TailCatcherCells"], OutputLevel = DEBUG)

from Configurables import GeoSvc
path_to_detector = '/afs/cern.ch/work/h/helsens/public/FCCsoft/FCCSW-0.8.3/'
detectors_to_use=[path_to_detector+'/Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                  path_to_detector+'/Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                  path_to_detector+'/Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                  path_to_detector+'/Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
                  path_to_detector+'/Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                  path_to_detector+'/Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
                  path_to_detector+'/Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml',
                  path_to_detector+'/Detector/DetFCChhTailCatcher/compact/FCChh_TailCatcher.xml',
                  path_to_detector+'/Detector/DetFCChhBaseline1/compact/FCChh_Solenoids.xml',
                  path_to_detector+'/Detector/DetFCChhBaseline1/compact/FCChh_Shielding.xml']

geoservice = GeoSvc("GeoSvc", detectors = detectors_to_use, OutputLevel = INFO)

#Configure tools for calo reconstruction
from Configurables import ReadNoiseFromFileTool, CombinedCaloTopoCluster
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
EMECcells = CellPositionsCaloDiscsTool("CellPositionsEMEC", 
                                       readoutName = ecalEndcapReadoutName, 
                                       mergedLayers = ecalEndcapNumberOfLayersToMerge, 
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
HECcells = CellPositionsCaloDiscsTool("CellPositionsHEC", 
                                      readoutName = hcalEndcapReadoutName, 
                                      mergedLayers = hcalEndcapNumberOfLayersToMerge, 
                                      OutputLevel = INFO)
HCalFwdcells = CellPositionsCaloDiscsTool("CellPositionsHCalFwd", 
                                          readoutName = hcalFwdReadoutName, 
                                          OutputLevel = INFO)
TailCatchercells = CellPositionsTailCatcherTool("CellPositionsTailCatcher", 
                                                readoutName = tailCatcherReadoutName, 
                                                centralRadius = 901.5,
                                                OutputLevel = INFO)

#Create topo clusters
createTopoClusters = CombinedCaloTopoCluster("CreateTopoClusters",
                                             ECalCells = "ECalBarrelCells",
                                             HCalCells = "HCalBarrelCells",
                                             ECalReadoutName = ecalBarrelReadoutNamePhiEta,
                                             HCalReadoutName = hcalBarrelReadoutName,
                                             noiseAddedECal = False,
                                             noiseAddedHCal = False,
#                                             noiseToolECal = noiseToolECal,
#                                             noiseToolHCal = noiseToolHCal,
                                             positionsToolECal = ECalBcells,
                                             positionsToolHCal = HCalBcells,
                                             neighboursRange = 1,
                                             lastECalLayer = 7,
                                             OutputLevel = INFO)
createTopoClusters.clusters.Path = "caloClusters"
createTopoClusters.clusterCells.Path = "caloClusterCells"

out = PodioOutput("out", filename = "~/FCCSW/condor/output_testTopoClusters_100GeVljets_ev1.root",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]#,"drop ECalHits"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createTopoClusters.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              createTopoClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [geoservice, podioevent, audsvc],
)
