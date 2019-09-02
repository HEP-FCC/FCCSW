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
ecalBarrelNoisePath = "/eos/project/f/fccsw-web/testsamples/elecAndPileupNoise_ecalBarrel_50Ohm_traces2_2shieldWidth.root" 
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
detectors_to_use =['file:Detector/DetFCChhBaseline1/compact/FCChh_DectMaster.xml',
                   ] 
geoservice = GeoSvc("GeoSvc", detectors = detectors_to_use, OutputLevel = INFO)

#Configure tools for calo reconstruction
from Configurables import ConstNoiseTool 
noiseTool = ConstNoiseTool("ConstNoiseTool")

#Configure tools for calo cell positions
from Configurables import CellPositionsECalBarrelTool,CellPositionsHCalBarrelTool, CellPositionsCaloDiscsTool,CellPositionsTailCatcherTool 
ECalBcells = CellPositionsECalBarrelTool("CellPositionsECalBarrel", 
                                         readoutName = ecalBarrelReadoutName, 
                                         OutputLevel = INFO)
EMECcells = CellPositionsCaloDiscsTool("CellPositionsEMEC", 
                                       readoutName = ecalEndcapReadoutName, 
                                       OutputLevel = INFO)
ECalFwdcells = CellPositionsCaloDiscsTool("CellPositionsECalFwd", 
                                          readoutName = ecalFwdReadoutName, 
                                          OutputLevel = INFO)
HCalBcells = CellPositionsHCalBarrelTool("CellPositionsHCalBarrel",
                                         readoutName = hcalBarrelReadoutName,
                                         radii = [291.05, 301.05, 313.55, 328.55, 343.55, 358.55, 378.55, 403.55, 428.55, 453.55],
                                         OutputLevel = INFO) 
HCalExtBcells =CellPositionsHCalBarrelTool("CellPositionsHCalExtBarrel",
                                           readoutName = hcalExtBarrelReadoutName,
                                           radii = [ 356.05
                                                     , 373.55
                                                     , 398.55
                                                     , 423.55
                                                     , 291.05
                                                     , 301.05
                                                     , 313.55
                                                     , 328.55
                                                     , 348.55
                                                     , 373.55
                                                     , 398.55
                                                     , 423.55
                                                     ],                                           
                                           OutputLevel = INFO) 
HECcells =CellPositionsCaloDiscsTool("CellPositionsHEC",
                                     readoutName = hcalEndcapReadoutName,
                                     OutputLevel = INFO) 
HCalFwdcells =CellPositionsCaloDiscsTool("CellPositionsHCalFwd",
                                         readoutName = hcalFwdReadoutName,
                                         OutputLevel = INFO)

#Configure tools for calo reconstruction                                                                                                                                                                       
from Configurables import RewriteBitfield, CalibrateCaloHitsTool, NoiseCaloCellsFlatTool, LayerPhiEtaCaloTool
calibHcells = CalibrateCaloHitsTool("CalibrateHCal", invSamplingFraction="41.7 ")
noise = NoiseCaloCellsFlatTool("HCalNoise",
                               cellNoise = 0.01)

rewriteHCal = RewriteBitfield("RewriteHCal",
                                # old bitfield (readout)
                                oldReadoutName = "HCalBarrelReadout",
                                # specify which fields are going to be deleted
                                removeIds = ["row"],
                                # new bitfield (readout), with new segmentation
                                newReadoutName = "BarHCal_Readout_phieta",
                                debugPrint = 10,
                                OutputLevel= INFO)
# clusters are needed, with deposit position and cellID in bits
rewriteHCal.inhits.Path = "HCalBarrelCells"
rewriteHCal.outhits.Path = "HCalBarrelCellsStep1"

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

createEcalBarrelCells = CreateCaloCells("CreateECalBarrelCells", 
                                        geometryTool = barrelGeometry, 
                                        doCellCalibration = False,
                                        #already calibrated 
                                        addCellNoise = True, 
                                        filterCellNoise = False,
                                        noiseTool = noiseBarrel, 
                                        hits = "ECalBarrelCells", 
                                        cells = "ECalBarrelCellsNoise")

#HCal Barrel noise
noiseHcal = NoiseCaloCellsFlatTool("HCalNoise", cellNoise = 0.01)

# Geometry for layer-eta-phi segmentation 
barrelHcalGeometry = LayerPhiEtaCaloTool("BarrelHcalGeo",
                                         readoutName = "BarHCal_Readout_phieta",
                                         activeVolumeName = "layerVolume",
                                         activeFieldName = "layer",
                                         fieldNames = ["system"],
                                         fieldValues = [8],
                                         activeVolumesNumber = 10,
                                         activeVolumesEta = [1.2524, 1.2234, 1.1956, 1.15609, 1.1189, 1.08397, 1.0509, 0.9999, 0.9534, 0.91072],
                                         OutputLevel= DEBUG)

createHcalBarrelCells =CreateCaloCells("CreateHCalBarrelCells", geometryTool = barrelHcalGeometry,
                                       doCellCalibration = False, addCellNoise = True,
                                       filterCellNoise = False, noiseTool = noiseHcal,
                                       OutputLevel = DEBUG) 
createHcalBarrelCells.hits.Path ="HCalBarrelCellsStep1" 
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
                                          hcalBarrelReadoutName = "BarHCal_Readout_phieta",
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
                                      fileName = "/eos/project/f/fccsw-web/testsamples/calo/neighbours_map_barrel.root",
                                      OutputLevel = DEBUG)

#Noise levels per cell
readNoisyCellsMap = TopoCaloNoisyCells("ReadNoisyCellsMap",
                                       fileName = "/eos/project/f/fccsw-web/testsamples/calo/cellNoise_map_electronicsNoiseLevel.root",
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
                rewriteHCal,
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
               OutputLevel = DEBUG
               )
