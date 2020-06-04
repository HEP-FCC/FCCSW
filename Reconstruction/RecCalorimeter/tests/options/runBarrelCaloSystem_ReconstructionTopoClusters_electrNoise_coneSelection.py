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
ecalBarrelNoisePath = "http://fccsw.web.cern.ch/fccsw/testsamples/elecNoise_ecalBarrel_50Ohm_traces2_2shieldWidth_noise.root" 
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
                   'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
                   'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                   ] 
geoservice = GeoSvc("GeoSvc", detectors = detectors_to_use, OutputLevel = INFO)

from Configurables import SimG4ParticleSmearFormula, SimG4SmearGenParticles
from FCChhTkLayoutResolutionFormula import momentumResolutionFormula 

# create particle smearing tool, used for smearing in the tracker
smeartool = SimG4ParticleSmearFormula()
smeartool.resolutionMomentum = momentumResolutionFormula

smear = SimG4SmearGenParticles("smear",
                               inParticles = "GenParticles",
                               smearedParticles = "SmearedParticles",
                               smearTool = smeartool,
                               OutputLevel=DEBUG)

#Configure tools for calo reconstruction
from Configurables import ConstNoiseTool 
noiseTool = ConstNoiseTool("ConstNoiseTool")

#Configure tools for calo cell positions
from Configurables import CellPositionsECalBarrelTool, CellPositionsHCalBarrelTool, CellPositionsHCalBarrelNoSegTool, CellPositionsDummyTool 
ECalBcells = CellPositionsECalBarrelTool("CellPositionsECalBarrel", 
                                         readoutName = ecalBarrelReadoutName)
HCalBcells = CellPositionsHCalBarrelTool("CellPositionsHCalBarrel",
                                            readoutName = "BarHCal_Readout_phieta",
                                            radii = [291.05, 301.05, 313.55, 328.55, 343.55, 358.55, 378.55, 413.55, 428.55, 453.55],
                                            OutputLevel = INFO)
dummyCells = CellPositionsDummyTool("CellPositions",
                                    OutputLevel = INFO)

#Configure tools for calo reconstruction                                                       
from Configurables import RewriteBitfield, CalibrateCaloHitsTool, NoiseCaloCellsFlatTool, LayerPhiEtaCaloTool, CreateCaloCells
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

createHcalCells = CreateCaloCells("CreateHCalCells",
                                  doCellCalibration=False,
                                  addCellNoise=False, filterCellNoise=False,
                                  hits="HCalBarrelCellsStep1",
                                  cells="newHCalBarrelCells")

from Configurables import NoiseCaloCellsFromFileTool, TubeLayerPhiEtaCaloTool, CalibrateCaloHitsTool, NoiseCaloCellsFlatTool, LayerPhiEtaCaloTool
#ECal Barrel noise
noiseBarrel = NoiseCaloCellsFromFileTool("NoiseBarrel",
                                         readoutName = ecalBarrelReadoutName,
                                         noiseFileName = ecalBarrelNoisePath,
                                         elecNoiseHistoName = ecalBarrelNoiseHistName,
                                         cellPositionsTool = dummyCells,
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

createEcalBarrelCells = CreateCaloCells("CreateECalBarrelCellsNoise",
                                        geometryTool = barrelGeometry,
                                        doCellCalibration=False, # already calibrated                          
                                        addCellNoise=True, filterCellNoise=False,
                                        noiseTool = noiseBarrel,
                                        hits = "ECalBarrelCells",
                                        cells = "ECalBarrelCellsNoise")

#HCal Barrel noise
noiseHcal = NoiseCaloCellsFlatTool("HCalNoise", cellNoise = 0.01)

barrelHcalGeometry = LayerPhiEtaCaloTool("BarrelHcalGeo",
                                         readoutName = "BarHCal_Readout_phieta",
                                         activeVolumeName = "layerVolume",
                                         activeFieldName = "layer",
                                         fieldNames = ["system"],
                                         fieldValues = [8],
                                         activeVolumesNumber = 10,
                                         activeVolumesEta = [1.2524, 1.2234, 1.1956, 1.15609, 1.1189, 1.08397, 1.0509, 0.9999, 0.9534, 0.91072]
                                         )

createHcalBarrelCells =CreateCaloCells("CreateHCalBarrelCells", geometryTool = barrelHcalGeometry,
                                       doCellCalibration = False, addCellNoise = True,
                                       filterCellNoise = False, noiseTool = noiseHcal) 
createHcalBarrelCells.hits.Path ="newHCalBarrelCells" 
createHcalBarrelCells.cells.Path ="HCalBarrelCellsNoise"

# Select cells before running clustering
from Configurables import ConeSelection 
selectionECal = ConeSelection("selectionECal",
                              cells = "ECalBarrelCellsNoise",
                              particles = "GenParticles",
                              selCells = "selectedECalBarrelCellsNoise",
                              positionsTool = ECalBcells,
                              radius = 0.4,
                              OutputLevel = INFO)
selectionHCal = ConeSelection("selectionHCal",
                              cells = "HCalBarrelCellsNoise",
                              particles = "GenParticles",
                              selCells = "selectedHCalBarrelCellsNoise",
                              positionsTool = HCalBcells,
                              radius = 0.4,
                              OutputLevel = INFO)

#Create topo clusters
from Configurables import CreateEmptyCaloCellsCollection 
createemptycells = CreateEmptyCaloCellsCollection("CreateEmptyCaloCells") 
createemptycells.cells.Path = "emptyCaloCells"

from Configurables import CaloTopoClusterInputTool, CaloTopoCluster, TopoCaloNeighbours, TopoCaloNoisyCells 
createTopoInput =CaloTopoClusterInputTool("CreateTopoInput",
                                          ecalBarrelReadoutName = ecalBarrelReadoutName,
                                          ecalEndcapReadoutName = "",
                                          ecalFwdReadoutName = "",
                                          hcalBarrelReadoutName = "BarHCal_Readout_phieta",
                                          hcalExtBarrelReadoutName = "",
                                          hcalEndcapReadoutName = "",
                                          hcalFwdReadoutName = "",
                                          OutputLevel = DEBUG) 
createTopoInput.ecalBarrelCells.Path ="selectedECalBarrelCellsNoise" 
createTopoInput.ecalEndcapCells.Path ="emptyCaloCells" 
createTopoInput.ecalFwdCells.Path ="emptyCaloCells" 
createTopoInput.hcalBarrelCells.Path ="selectedHCalBarrelCellsNoise" 
createTopoInput.hcalExtBarrelCells.Path ="emptyCaloCells" 
createTopoInput.hcalEndcapCells.Path ="emptyCaloCells" 
createTopoInput.hcalFwdCells.Path = "emptyCaloCells"

readNeighboursMap =TopoCaloNeighbours("ReadNeighboursMap",
                                      fileName = "http://fccsw.web.cern.ch/fccsw/testsamples/calo/neighbours_map_barrel.root",
                                      OutputLevel = DEBUG)

#Noise levels per cell
readNoisyCellsMap = TopoCaloNoisyCells("ReadNoisyCellsMap",
#                                       fileName = "http://fccsw.web.cern.ch/fccsw/testsamples/calo/cellNoise_map_electronicsNoiseLevel.root",
                                       fileName = "http://fccsw.web.cern.ch/fccsw/testsamples/calo/cellNoise_map_segHcal_electronicsNoiseLevel.root",
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
                                     positionsHCalExtBarrelTool = dummyCells,
                                     positionsEMECTool = dummyCells,
                                     positionsHECTool = dummyCells,
                                     positionsEMFwdTool = dummyCells,
                                     positionsHFwdTool = dummyCells,
                                     seedSigma = 4,
                                     neighbourSigma = 2,
                                     lastNeighbourSigma = 0,
                                     noSegmentationHCal = False,
                                     OutputLevel = INFO) 
createTopoClusters.clusters.Path ="caloClustersBarrel" 
createTopoClusters.clusterCells.Path = "caloClusterBarrelCells"

from Configurables import SplitClusters

splitClusters = SplitClusters("splitClusters",
                              clusters = "caloClustersBarrel",
                              outClusters = "splitCaloClusters",
                              outCells = "splitCaloClusterCells",
                              readoutECal = ecalBarrelReadoutName,
                              readoutHCal = "BarHCal_Readout_phieta",
                              #expects neighbours map from cellid->vec < neighbourIds >                                                                 
                              neigboursTool = readNeighboursMap,
                              threshold = 0.01,
                              OutputLevel=INFO)

#Fill a collection of CaloHitPositions for detailed Cluster analysis
from Configurables import CreateCaloCellPositions 
positionsClusterBarrel =CreateCaloCellPositions("positionsClusterBarrel",
                                                positionsECalBarrelTool = ECalBcells,
                                                positionsHCalBarrelTool = HCalBcells,
                                                positionsHCalExtBarrelTool = dummyCells,
                                                positionsEMECTool = dummyCells,
                                                positionsHECTool = dummyCells,
                                                positionsEMFwdTool = dummyCells,
                                                positionsHFwdTool = dummyCells,
                                                hits = "splitCaloClusterCells",
                                                positionedHits = "caloClusterBarrelCellPositions",
                                                OutputLevel = INFO)


out = PodioOutput("out", filename = "output_BarrelTopo_electrNoise_100GeVpi_3ev.root", OutputLevel = DEBUG)
out.outputCommands =["drop *", "keep GenParticles", "keep GenVertices", "keep caloClustersBarrel","keep caloClusterBarrelCells","keep splitCaloClusters", "keep splitCaloClusterCells", "keep caloClusterBarrelCellPositions"]

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
                smear,
                rewriteHCal,
                createHcalCells,
                createEcalBarrelCells, 
                createHcalBarrelCells,
                createemptycells, 
                selectionECal,
                selectionHCal,
                createTopoClusters, 
                splitClusters,
#                positionsClusterBarrel,
                out
                ],
               EvtSel = 'NONE',
               EvtMax = num_events,
               ExtSvc = [ geoservice, podioevent, audsvc ],
               #OutputLevel = VERBOSE
               )
