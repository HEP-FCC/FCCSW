# Setup
# Names of cells collections
ecalBarrelCellsName = "ECalBarrelCells"
ecalEndcapCellsName = "ECalEndcapCells"
ecalFwdCellsName = "ECalFwdCells"
hcalBarrelCellsName = "HCalBarrelCells"
hcalExtBarrelCellsName = "HCalExtBarrelCells"
hcalEndcapCellsName = "HCalEndcapCells"
hcalFwdCellsName = "HCalFwdCells"
# Readouts
ecalBarrelReadoutName = "ECalBarrelPhiEta"
ecalEndcapReadoutName = "EMECPhiEtaReco"
ecalFwdReadoutName = "EMFwdPhiEta"
hcalBarrelReadoutName = "BarHCal_Readout_phieta"
hcalExtBarrelReadoutName = "HCalExtBarrelReadout"
hcalBarrelReadoutPhiEtaName = "BarHCal_Readout_phieta"
hcalExtBarrelReadoutPhiEtaName = "ExtBarHCal_Readout_phieta"
hcalEndcapReadoutName = "HECPhiEta"
hcalFwdReadoutName = "HFwdPhiEta"
# Number of events
num_events = 3

from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input="output_fullCalo_SimAndDigi_pi50GeV_"+str(num_events)+"events.root")
# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader",
                        collections = [ecalBarrelCellsName, ecalEndcapCellsName, ecalFwdCellsName,
                                       hcalBarrelCellsName, hcalExtBarrelCellsName, hcalEndcapCellsName, hcalFwdCellsName])

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           # Tracker disabled to save cpu time
                                           #'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                                           'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
                                           'file:Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml'],
                    OutputLevel = WARNING)


ecalBarrelNoisePath = "http://fccsw.web.cern.ch/fccsw/testsamples/elecNoise_ecalBarrel_50Ohm_traces2_2shieldWidth.root"
ecalEndcapNoisePath = "http://fccsw.web.cern.ch/fccsw/testsamples/elecNoise_emec_50Ohm_2shieldWidth_6layers.root"
ecalBarrelNoiseHistName = "h_elecNoise_fcc_"
ecalEndcapNoiseHistName = "h_elecNoise_fcc_"

from Configurables import CreateVolumeCaloPositions, RedoSegmentation, NoiseCaloCellsFlatTool
positionsExtHcal = CreateVolumeCaloPositions("positionsExtHcal", OutputLevel = INFO)
positionsExtHcal.hits.Path = hcalExtBarrelCellsName
positionsExtHcal.positionedHits.Path = "HCalExtBarrelPositions"

resegmentExtHcal = RedoSegmentation("ReSegmentationExtHcal",
                                # old bitfield (readout)
                                oldReadoutName = hcalExtBarrelReadoutName,
                                # specify which fields are going to be altered (deleted/rewritten)
                                oldSegmentationIds = ["module","row"],
                                # new bitfield (readout), with new segmentation
                                newReadoutName = hcalExtBarrelReadoutPhiEtaName,
                                debugPrint = 10,
                                OutputLevel = INFO,
                                inhits = "HCalExtBarrelPositions",
                                outhits = "newHCalExtBarrelCells")

##############################################################################################################
#######                                       REWRITE ENDCAP BITFIELD                            #############
##############################################################################################################

from Configurables import RewriteBitfield
rewriteECalEC = RewriteBitfield("RewriteECalEC",
                                # old bitfield (readout)
                                oldReadoutName = "EMECPhiEta",
                                # specify which fields are going to be deleted
                                removeIds = ["sublayer"],
                                # new bitfield (readout), with new segmentation
                                newReadoutName = ecalEndcapReadoutName,
                                debugPrint = 10,
                                OutputLevel= INFO)
# clusters are needed, with deposit position and cellID in bits
rewriteECalEC.inhits.Path = "ECalEndcapCells"
rewriteECalEC.outhits.Path = "newECalEndcapCells"

rewriteHCalEC = RewriteBitfield("RewriteHCalEC",
                                # old bitfield (readout)
                                oldReadoutName = "HECPhiEta",
                                # specify which fields are going to be deleted
                                removeIds = ["sublayer"],
                                # new bitfield (readout), with new segmentation
                                newReadoutName = hcalEndcapReadoutName,
                                debugPrint = 10,
                                OutputLevel = INFO)
# clusters are needed, with deposit position and cellID in bits
rewriteHCalEC.inhits.Path = "HCalEndcapCells"
rewriteHCalEC.outhits.Path = "newHCalEndcapCells"

# add noise, create all existing cells in detector
from Configurables import NoiseCaloCellsFromFileTool, TubeLayerPhiEtaCaloTool, LayerPhiEtaCaloTool, CreateCaloCells
noiseBarrel = NoiseCaloCellsFromFileTool("NoiseBarrel",
                                         readoutName = ecalBarrelReadoutName,
                                         noiseFileName = ecalBarrelNoisePath,
                                         elecNoiseHistoName = ecalBarrelNoiseHistName,
                                         activeFieldName = "layer",
                                         addPileup = False,
                                         numRadialLayers = 8)
barrelGeometry = TubeLayerPhiEtaCaloTool("EcalBarrelGeo",
                                         readoutName = ecalBarrelReadoutName,
                                         activeVolumeName = "LAr_sensitive",
                                         activeFieldName = "layer",
                                         fieldNames = ["system"],
                                         fieldValues = [5],
                                         activeVolumesNumber = 8)
createEcalBarrelCells = CreateCaloCells("CreateECalBarrelCells",
                                        geometryTool = barrelGeometry,
                                        doCellCalibration=False, # already calibrated
                                        addCellNoise=True, filterCellNoise=False,
                                        noiseTool = noiseBarrel,
                                        hits=ecalBarrelCellsName,
                                        cells=ecalBarrelCellsName+"Noise",
                                        OutputLevel=DEBUG)

noiseHcal = NoiseCaloCellsFlatTool("HCalNoise", cellNoise = 0.009)

barrelHcalGeometry = LayerPhiEtaCaloTool("HcalBarrelGeo",
                                         readoutName = hcalBarrelReadoutName,
                                         activeVolumeName = "layerVolume",
                                         activeFieldName = "layer",
                                         fieldNames = ["system"],
                                         fieldValues = [8],
                                         activeVolumesNumber = 10)
createHcalBarrelCells = CreateCaloCells("CreateHCalBarrelCells",
                                        geometryTool = barrelHcalGeometry,
                                        doCellCalibration=False, # already calibrated
                                        addCellNoise=True, filterCellNoise=False,
                                        noiseTool = noiseHcal,
                                        hits = hcalBarrelCellsName,
                                        cells = hcalBarrelCellsName+"Noise",
                                        OutputLevel=DEBUG)

# add noise, create all existing cells in detector
# currently only positive side!
noiseEndcap = NoiseCaloCellsFromFileTool("NoiseEndcap",
                                                 readoutName = ecalEndcapReadoutName,
                                                 noiseFileName = ecalEndcapNoisePath,
                                                 elecNoiseHistoName = ecalEndcapNoiseHistName,
                                                 activeFieldName = "layer",
                                                 addPileup = False,
                                                 numRadialLayers = 6)
endcapGeometry = TubeLayerPhiEtaCaloTool("EcalEndcapGeo",
                                                 readoutName = ecalEndcapReadoutName,
                                                 activeVolumeName = "layerEnvelope",
                                                 activeFieldName = "layer",
                                                 activeVolumesNumber = 6,
                                                 fieldNames = ["system"],
                                                 fieldValues = [6])
createEcalEndcapCells = CreateCaloCells("CreateECalEndcapCells",
                                                geometryTool = endcapGeometry,
                                                doCellCalibration=False, # already calibrated
                                                addCellNoise=True, filterCellNoise=False,
                                                noiseTool = noiseEndcap,
                                                hits="new"+ecalEndcapCellsName,
                                                cells=ecalEndcapCellsName+"Noise")

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, CaloTowerTool
from GaudiKernel.PhysicalConstants import pi

towers = CaloTowerTool("towers",
                               deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                               ecalBarrelReadoutName = ecalBarrelReadoutName,
                               ecalEndcapReadoutName = ecalEndcapReadoutName,
                               ecalFwdReadoutName = ecalFwdReadoutName,
                               hcalBarrelReadoutName = hcalBarrelReadoutPhiEtaName,
                               hcalExtBarrelReadoutName = hcalExtBarrelReadoutPhiEtaName,
                               hcalEndcapReadoutName = hcalEndcapReadoutName,
                               hcalFwdReadoutName = hcalFwdReadoutName,
                               OutputLevel = DEBUG)
towers.ecalBarrelCells.Path = ecalBarrelCellsName + "Noise"
towers.ecalEndcapCells.Path = ecalEndcapCellsName + "Noise"
towers.ecalFwdCells.Path = ecalFwdCellsName
towers.hcalBarrelCells.Path = hcalBarrelCellsName + "Noise"
towers.hcalExtBarrelCells.Path ="newHCalExtBarrelCells"
towers.hcalEndcapCells.Path = hcalEndcapCellsName
towers.hcalFwdCells.Path = hcalFwdCellsName

# Cluster variables
windE = 9
windP = 17
posE = 5
posP = 11
dupE = 7
dupP = 13
finE = 9
finP = 17
threshold = 12

createClusters = CreateCaloClustersSlidingWindow("CreateClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = windE, nPhiWindow = windP,
                                                 nEtaPosition = posE, nPhiPosition = posP,
                                                 nEtaDuplicates = dupE, nPhiDuplicates = dupP,
                                                 nEtaFinal = finE, nPhiFinal = finP,
                                                 energyThreshold = threshold)
createClusters.clusters.Path = "CaloClusters"
createClusters.clusterCells.Path = "CaloClusterCells"

out = PodioOutput("out", filename="output_allCalo_reco_noise.root")
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createClusters.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              rewriteECalEC,
              rewriteHCalEC,
              positionsExtHcal,
              resegmentExtHcal,
              createEcalBarrelCells,
              createHcalBarrelCells,
              createEcalEndcapCells,
              createClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = num_events,
    ExtSvc = [podioevent, geoservice])

