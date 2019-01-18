#Setup
#Names of cells collections
ecalBarrelCellsName ="ECalBarrelCells" 
hcalBarrelCellsName = "HCalBarrelCells"
#ECAL readouts
ecalBarrelReadoutName = "ECalBarrelPhiEta" 
ecalEndcapReadoutName = "EMECPhiEtaReco" 
ecalFwdReadoutName ="EMFwdPhiEtaReco"
#HCAL readouts
hcalBarrelReadoutName = "HCalBarrelReadout" 
hcalExtBarrelReadoutName = "HCalExtBarrelReadout" 
hcalEndcapReadoutName = "HECPhiEtaReco" 
hcalFwdReadoutName = "HFwdPhiEtaReco"

#Number of events
num_events = 3

#Geometry details to add noise to every Calo cell and paths to root files that have the noise const per cell
ecalBarrelNoisePath = "/afs/cern.ch/user/a/azaborow/public/FCCSW/elecNoise_ecalBarrel_50Ohm_traces2_2shieldWidth_noise.root" 
ecalBarrelNoiseHistName ="h_elecNoise_fcc_"
#active material identifier name
hcalIdentifierName = [ "module", "row", "layer" ]
#active material volume name
hcalVolumeName = [ "moduleVolume", "wedgeVolume", "layerVolume" ]
#ECAL bitfield names& values
hcalFieldNames = ["system"] 
hcalFieldValues = [8]

from Gaudi.Configuration import *
from Configurables import ApplicationMgr,FCCDataSvc,PodioOutput
podioevent = FCCDataSvc("EventDataSvc", input = "output_fullCalo_SimAndDigi_e50GeV_" +str(num_events) + "events.root")

#reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput 
podioinput = PodioInput("PodioReader", collections =[
        "ECalBarrelCells", "HCalBarrelCells", "HCalExtBarrelCells",
        "GenParticles", "GenVertices"
        ],
                        OutputLevel = DEBUG)

from Configurables import GeoSvc 
detectors_to_use =['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                   'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                   'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'
                   ] 
geoservice = GeoSvc("GeoSvc", detectors = detectors_to_use, OutputLevel = INFO)

#Configure tools for calo reconstruction
from Configurables import ConstNoiseTool 
noiseTool = ConstNoiseTool("ConstNoiseTool")

#Configure tools for calo cell positions
from Configurables import CellPositionsECalBarrelTool,CellPositionsHCalBarrelNoSegTool, CellPositionsCaloDiscsTool,CellPositionsTailCatcherTool 
ECalBcells = CellPositionsECalBarrelTool("CellPositionsECalBarrel", 
                                         readoutName = ecalBarrelReadoutName, 
                                         OutputLevel = INFO)
EMECcells = CellPositionsCaloDiscsTool("CellPositionsEMEC", 
                                       readoutName = ecalEndcapReadoutName, 
                                       OutputLevel = INFO)
ECalFwdcells = CellPositionsCaloDiscsTool("CellPositionsECalFwd", 
                                          readoutName = ecalFwdReadoutName, 
                                          OutputLevel = INFO)
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

from Configurables import CreateCaloCells,NoiseCaloCellsFromFileTool, TubeLayerPhiEtaCaloTool, CalibrateCaloHitsTool,NoiseCaloCellsFlatTool,NestedVolumesCaloTool
#ECal Barrel noise
noiseBarrel = NoiseCaloCellsFromFileTool("NoiseBarrel",
                                         readoutName = ecalBarrelReadoutName,
                                         noiseFileName = ecalBarrelNoisePath,
                                         elecNoiseHistoName = ecalBarrelNoiseHistName,
                                         activeFieldName = "layer",
                                         addPileup = False,
                                         numRadialLayers = 8)

#add noise, create all existing cells in detector
barrelGeometry = TubeLayerPhiEtaCaloTool("EcalBarrelGeo",
                                         readoutName = ecalBarrelReadoutName,
                                         activeVolumeName = "LAr_sensitive",
                                         activeFieldName = "layer",
                                         fieldNames = ["system"],
                                         fieldValues = [5],
                                         activeVolumesNumber = 8)

createEcalBarrelCells = CreateCaloCells(
    "CreateECalBarrelCells", geometryTool = barrelGeometry, doCellCalibration = False,
    #already calibrated addCellNoise = True, filterCellNoise = False,
    noiseTool = noiseBarrel, hits = "ECalBarrelCells", cells = "ECalBarrelCellsNoise")

#HCal Barrel noise
noiseHcal = NoiseCaloCellsFlatTool("HCalNoise", cellNoise = 0.009)

hcalgeo = NestedVolumesCaloTool("HcalGeo",
                                activeVolumeName = hcalVolumeName,
                                activeFieldName = hcalIdentifierName,
                                readoutName = hcalBarrelReadoutName,
                                fieldNames = hcalFieldNames,
                                fieldValues = hcalFieldValues,
                                OutputLevel = INFO)

createHcalBarrelCells =CreateCaloCells("CreateHCalBarrelCells", geometryTool = hcalgeo,
                                       doCellCalibration = False, addCellNoise = True,
                                       filterCellNoise = False, noiseTool = noiseHcal,
                                       OutputLevel = INFO) 
createHcalBarrelCells.hits.Path ="HCalBarrelCells" 
createHcalBarrelCells.cells.Path ="HCalBarrelCellsNoise"

#Create topo clusters
from Configurables import CreateEmptyCaloCellsCollection 
createemptycells = CreateEmptyCaloCellsCollection("CreateEmptyCaloCells") 
createemptycells.cells.Path = "emptyCaloCells"

from Configurables import CaloTopoClusterInputTool,CaloTopoCluster, TopoCaloNeighbours,TopoCaloNoisyCells 
createTopoInput =CaloTopoClusterInputTool("CreateTopoInput",
                                          ecalBarrelReadoutName = ecalBarrelReadoutName,
                                          ecalEndcapReadoutName = "",
                                          ecalFwdReadoutName = "",
                                          hcalBarrelReadoutName = hcalBarrelReadoutName,
                                          hcalExtBarrelReadoutName = "",
                                          hcalEndcapReadoutName = "",
                                          hcalFwdReadoutName = "",
                                          OutputLevel = DEBUG) 
createTopoInput.ecalBarrelCells.Path ="ECalBarrelCellsNoise" 
createTopoInput.ecalEndcapCells.Path ="emptyCaloCells" 
createTopoInput.ecalFwdCells.Path ="emptyCaloCells" 
createTopoInput.hcalBarrelCells.Path ="HCalBarrelCellsNoise" 
createTopoInput.hcalExtBarrelCells.Path ="emptyCaloCells" 
createTopoInput.hcalEndcapCells.Path ="emptyCaloCells" 
createTopoInput.hcalFwdCells.Path = "emptyCaloCells"

readNeighboursMap =TopoCaloNeighbours("ReadNeighboursMap",
                                      fileName = "/afs/cern.ch/work/c/cneubuse/public/FCChh/neighbours_map_segHcal.root",
                                      OutputLevel = DEBUG)

#Noise levels per cell
readNoisyCellsMap = TopoCaloNoisyCells("ReadNoisyCellsMap",
                                       fileName = "/afs/cern.ch/work/c/cneubuse/public/FCChh/"
                                       "cellNoise_map_segHcal_electronicsNoiseLevel.root",
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
                                     neighbourSigma = 2,
                                     lastNeighbourSigma = 0,
                                     OutputLevel = DEBUG) 
createTopoClusters.clusters.Path ="caloClustersBarrel" 
createTopoClusters.clusterCells.Path = "caloClusterBarrelCells"

#Fill a collection of CaloHitPositions for detailed Cluster analysis
from Configurables import CreateCaloCellPositions 
positionsClusterBarrel =CreateCaloCellPositions("positionsClusterBarrel",
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

out = PodioOutput("out", filename = "output_BarrelTopo_electrNoise_50GeVe_3ev.root", OutputLevel = DEBUG)
out.outputCommands =["drop *", "keep GenParticles", "keep GenVertices", "keep caloClustersBarrel","keep caloClusterBarrelCells", "keep caloClusterBarrelCellPositions"]

#CPU information
from Configurables import AuditorSvc,ChronoAuditor 
chra = ChronoAuditor() 
audsvc = AuditorSvc() 
audsvc.Auditors =[chra] 
podioinput.AuditExecute = True 
createemptycells.AuditExecute =True 
createEcalBarrelCells.AuditExecute = True 
createHcalBarrelCells.AuditExecute =True 
createTopoClusters.AuditExecute = True 
positionsClusterBarrel.AuditExecute = True

ApplicationMgr(TopAlg =
               [podioinput, 
                createEcalBarrelCells, 
                createHcalBarrelCells,
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
