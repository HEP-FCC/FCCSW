#Setup
#Names of cells collections
ecalBarrelCellsName ="ECalBarrelCells" 
hcalBarrelCellsName = "HCalBarrelCells"
#ECAL readouts
ecalBarrelReadoutName = "ECalBarrelPhiEta" 
ecalEndcapReadoutName = "EMECPhiEtaReco" 
ecalFwdReadoutName = "EMFwdPhiEtaReco"
#HCAL readouts
hcalBarrelReadoutName = "HCalBarrelReadout" 
hcalExtBarrelReadoutName ="HCalExtBarrelReadout" 
hcalEndcapReadoutName = "HECPhiEtaReco" 
hcalFwdReadoutName = "HFwdPhiEtaReco"

#Number of events
num_events = 3

from Gaudi.Configuration import *
from Configurables import ApplicationMgr,FCCDataSvc,PodioOutput
podioevent = FCCDataSvc("EventDataSvc", input = "output_fullCalo_SimAndDigi_e50GeV_" +
                        str(num_events) + "events.root")

#reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput 
podioinput = PodioInput("PodioReader", collections =  [ "ECalBarrelCells", "HCalBarrelCells", "HCalExtBarrelCells",
                                                        "GenParticles", "GenVertices"
                                                        ],
                        OutputLevel = DEBUG)

from Configurables import GeoSvc 
detectors_to_use =['file:Detector/DetFCChhBaseline1/compact/FCChh_DectMaster.xml',
                   ] 
geoservice = GeoSvc("GeoSvc", detectors = detectors_to_use, OutputLevel = INFO)

#Configure tools for calo reconstruction
from Configurables import ConstNoiseTool 
noiseTool =ConstNoiseTool("ConstNoiseTool")

#Configure tools for calo cell positions
from Configurables import CellPositionsECalBarrelTool,CellPositionsHCalBarrelNoSegTool, CellPositionsCaloDiscsTool
ECalBcells = CellPositionsECalBarrelTool(
    "CellPositionsECalBarrel", readoutName = ecalBarrelReadoutName, OutputLevel = INFO)
EMECcells = CellPositionsCaloDiscsTool(
    "CellPositionsEMEC", readoutName = ecalEndcapReadoutName, OutputLevel = INFO)
ECalFwdcells = CellPositionsCaloDiscsTool(
    "CellPositionsECalFwd", readoutName = ecalFwdReadoutName, OutputLevel = INFO)
HCalBcells = CellPositionsHCalBarrelNoSegTool("CellPositionsHCalBarrelVols",
                                              readoutName = hcalBarrelReadoutName,
                                              OutputLevel = INFO) 
HCalExtBcells =CellPositionsHCalBarrelNoSegTool("CellPositionsHCalExtBarrel",
                                                readoutName = hcalExtBarrelReadoutName,
                                                OutputLevel = INFO) 
HECcells =CellPositionsCaloDiscsTool("CellPositionsHEC",
                                     readoutName = hcalEndcapReadoutName,
                                     OutputLevel = INFO) 
HCalFwdcells =CellPositionsCaloDiscsTool("CellPositionsHCalFwd",
                                         readoutName = hcalFwdReadoutName,
                                         OutputLevel = INFO)

#Create topo clusters
from Configurables import CreateEmptyCaloCellsCollection
createemptycells =CreateEmptyCaloCellsCollection("CreateEmptyCaloCells")
createemptycells.cells.Path = "emptyCaloCells"

from Configurables import CaloTopoClusterInputTool,CaloTopoCluster, TopoCaloNeighbours, TopoCaloNoisyCells 
createTopoInput = CaloTopoClusterInputTool("CreateTopoInput",
                                           ecalBarrelReadoutName = ecalBarrelReadoutName,
                                           ecalEndcapReadoutName = "",
                                           ecalFwdReadoutName = "",
                                           hcalBarrelReadoutName = hcalBarrelReadoutName,
                                           hcalExtBarrelReadoutName = "",
                                           hcalEndcapReadoutName = "",
                                           hcalFwdReadoutName = "",
                                           OutputLevel = DEBUG) 
createTopoInput.ecalBarrelCells.Path ="ECalBarrelCells" 
createTopoInput.ecalEndcapCells.Path ="emptyCaloCells" 
createTopoInput.ecalFwdCells.Path ="emptyCaloCells" 
createTopoInput.hcalBarrelCells.Path = "HCalBarrelCells" 
createTopoInput.hcalExtBarrelCells.Path ="emptyCaloCells" 
createTopoInput.hcalEndcapCells.Path ="emptyCaloCells" 
createTopoInput.hcalFwdCells.Path = "emptyCaloCells"

readNeighboursMap =TopoCaloNeighbours("ReadNeighboursMap",
                                      fileName = "http://fccsw.web.cern.ch/fccsw/testsamples/calo/neighbours_map_segHcal.root",
                                      OutputLevel = DEBUG)

#Thresholds estimated from atlas, without noise !!!
readNoisyCellsMap = TopoCaloNoisyCells(
    "ReadNoisyCellsMap",
    fileName =
    "http://fccsw.web.cern.ch/fccsw/testsamples/calo/cellNoise_map_segHcal_constNoiseLevel.root",
    OutputLevel = DEBUG)

createTopoClusters = CaloTopoCluster("CreateTopoClusters",
                                     TopoClusterInput = createTopoInput,
                                     #expects neighbours map from cellid->vec < neighbourIds >
                                     neigboursTool = readNeighboursMap,
                                     #tool to get noise level per cellid
                                     noiseTool = readNoisyCellsMap,
                                     #cell positions tools for all sub - systems
                                     positionsECalBarrelTool = ECalBcells,
                                     positionsHCalBarrelTool = HCalBcells,
                                     positionsHCalExtBarrelTool = HCalExtBcells,
                                     positionsEMECTool = EMECcells,
                                     positionsHECTool = HECcells,
                                     positionsEMFwdTool = ECalFwdcells,
                                     positionsHFwdTool = HCalFwdcells,
                                     seedSigma = 4,
                                     neighbourSigma = 0,
                                     lastNeighbourSigma = 0,
                                     OutputLevel = DEBUG) 
createTopoClusters.clusters.Path ="caloClustersBarrel" 
createTopoClusters.clusterCells.Path = "caloClusterBarrelCells"

#Fill a collection of CaloHitPositions for detailed Cluster analysis
from Configurables import CreateCaloCellPositions 
positionsClusterBarrel = CreateCaloCellPositions("positionsClusterBarrel",
                                                 positionsECalBarrelTool = ECalBcells,
                                                 positionsHCalBarrelTool = HCalBcells,
                                                 positionsHCalExtBarrelTool = HCalExtBcells,
                                                 positionsEMECTool = EMECcells,
                                                 positionsHECTool = HECcells,
                                                 positionsEMFwdTool = ECalFwdcells,
                                                 positionsHFwdTool = HCalFwdcells,
                                                 hits = "caloClusterBarrelCells",
                                                 positionedHits = "caloClusterBarrelCellPositions",
                                                 OutputLevel = INFO)

out = PodioOutput("out", filename = "output_BarrelTopo_50GeVe_3ev.root", OutputLevel = DEBUG)
out.outputCommands =["drop *", "keep GenParticles", "keep GenVertices", "keep caloClustersBarrel",
                     "keep caloClusterBarrelCells", "keep caloClusterBarrelCellPositions"
                     ]

#CPU information
from Configurables import AuditorSvc,ChronoAuditor 
chra = ChronoAuditor() 
audsvc = AuditorSvc() 
audsvc.Auditors = [chra] 
podioinput.AuditExecute = True 
createemptycells.AuditExecute = True 
createTopoClusters.AuditExecute = True 
positionsClusterBarrel.AuditExecute = True 
out.AuditExecute = True

ApplicationMgr(TopAlg =[podioinput, 
                        createemptycells, 
                        createTopoClusters,
                        positionsClusterBarrel, 
                        out
                        ],
               EvtSel = 'NONE',
               EvtMax = 3,
               ExtSvc = [ geoservice, podioevent, audsvc ],
               #OutputLevel = VERBOSE
               )
