num_events = 10
energy=20
parti=3
bfield=1
suffix="_eta0_v1"

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input="output.root")
#podioevent = FCCDataSvc("EventDataSvc", input="root://eospublic//eos/experiment/fcc/users/n/novaj/afterBerlin_June/output_combCalo_e"+str(energy)+"GeV_bfield"+str(bfield)+"_part"+str(parti)+suffix+".root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections = ["ECalPositions", "GenParticles", "GenVertices"], OutputLevel = DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'
                                           ],
                    OutputLevel = DEBUG)

# common ECAL specific information
# readout name
ecalReadoutName = "ECalHitsEta"
ecalReadoutNamePhiEta = "ECalHitsPhiEta"
# active material identifier name
ecalIdentifierName = "layer"
# active material volume name
ecalVolumeName = "layer"
# ECAL bitfield names & values
ecalFieldNames=["system"]
ecalFieldValues=[5]
#ecalFieldNames=["system"]                                                                                                                           
#ecalFieldValues=[5]
# Number of layers to be merged in each layer
ecalNumberOfLayersToMerge=[4] + [18] * 7      
# number of ECAL layers
ecalNumberOfLayers = len(ecalNumberOfLayersToMerge)
# common HCAL specific information
# readout name
hcalReadoutNamePhiEta = "BarHCal_Readout_phieta_offset"

# Prepare ecal cells
# Resegment to phi-eta grid
from Configurables import RedoSegmentation
resegmentEcal = RedoSegmentation("ReSegmentationEcal",
                             # old bitfield (readout)
                             oldReadoutName = ecalReadoutName,
                             # # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = ["module"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = ecalReadoutNamePhiEta,
                             OutputLevel = INFO,
                             inhits = "ECalPositions",
                             outhits = "ECalCellsStep1")

# Merge layers
from Configurables import MergeLayers
mergeLayersEcal = MergeLayers("MergeLayersEcal",
                   # take the bitfield description from the geometry service
                   readout = ecalReadoutNamePhiEta,
                   # cells in which field should be merged
                   identifier = ecalIdentifierName,
                   volumeName = ecalVolumeName,
                   # how many cells to merge
                   merge = ecalNumberOfLayersToMerge,
                   OutputLevel = INFO)
mergeLayersEcal.inhits.Path = "ECalCellsStep1"
mergeLayersEcal.outhits.Path = "ECalCellsStep2"

# Configure tools for calo reconstruction                                       
from Configurables import CalibrateInLayersTool
calibEcells = CalibrateInLayersTool("Calibrate",
                                    # sampling fraction obtained using SamplingFractionInLayers from DetStudies package
                                    samplingFraction = [0.121251526526, 0.142826263372, 0.163536997242, 0.176624739048, 0.18867238304, 0.198904929474, 0.206372296332, 0.208016951216],
                                    readoutName = ecalReadoutNamePhiEta,
                                    layerFieldName = "layer")

#Configure tools for calo reconstruction
from Configurables import NoiseCaloCellsFlatTool, TubeLayerPhiEtaCaloTool
noiseEcells = NoiseCaloCellsFlatTool("NoiseCaloCellsFlatTool",
                               cellNoise = 0.001);
ecalgeo = TubeLayerPhiEtaCaloTool("EcalGeo",
                                  readoutName = ecalReadoutNamePhiEta,
                                  activeVolumeName = ecalVolumeName,
                                  activeFieldName = ecalIdentifierName,
                                  fieldNames = ecalFieldNames,
                                  fieldValues = ecalFieldValues,
                                  activeVolumesNumber = ecalNumberOfLayers,
                                  OutputLevel = DEBUG)

from Configurables import CreateCaloCells
createCellsEcal = CreateCaloCells("CreateCaloCellsEcal",
                              geometryTool = ecalgeo,
                              doCellCalibration = True,
                              calibTool = calibEcells,
                              addCellNoise = True, filterCellNoise = False,
                              noiseTool = noiseEcells,
                              OutputLevel = DEBUG)
createCellsEcal.hits.Path = "ECalCellsStep2"
createCellsEcal.cells.Path = "ECalCellsForSW"

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, SingleCaloTowerTool, CombinedCaloTowerTool
from GaudiKernel.PhysicalConstants import pi

towersEcal = SingleCaloTowerTool("towersEcal",
                             deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                             readoutName = ecalReadoutNamePhiEta,
                             OutputLevel = DEBUG)
towersEcal.cells.Path = "ECalCellsForSW"

windE = 9
windP = 17
posE = 5
posP = 11
dupE = 7
dupP = 13
finE = 9
finP = 17
thresholdE = 12
thresholdH = 5
createEcalClusters = CreateCaloClustersSlidingWindow("CreateEcalClusters",
                                                 towerTool = towersEcal,
                                                 nEtaWindow = windE, nPhiWindow = windP,
                                                 nEtaPosition = posE, nPhiPosition = posP,
                                                 nEtaDuplicates = dupE, nPhiDuplicates = dupP,
                                                 nEtaFinal = finE, nPhiFinal = finP,
                                                 energyThreshold = thresholdE,
                                                 OutputLevel = DEBUG)
createEcalClusters.clusters.Path = "EcalClusters"

out = PodioOutput("out", filename = "outputSW.root", OutputLevel=DEBUG)
out.outputCommands = ["keep *","drop ECalPositionedHits", "drop ECalCellsStep1", "drop ECalCellsStep2"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
resegmentEcal.AuditExecute = True
createEcalClusters.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              resegmentEcal,
              mergeLayersEcal,
              createCellsEcal,
              createEcalClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = int(num_events),
    ExtSvc = [geoservice, podioevent, audsvc],
 )
