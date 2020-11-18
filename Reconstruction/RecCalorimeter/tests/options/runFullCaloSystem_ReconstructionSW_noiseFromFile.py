# Setup


#CPU information
from Configurables import ChronoAuditor
chra = ChronoAuditor()
from Configurables import AuditorSvc
audsvc = AuditorSvc()
audsvc.Auditors = [chra]

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
hcalBarrelReadoutName = "HCalBarrelReadout"
hcalExtBarrelReadoutName = "HCalExtBarrelReadout"
hcalBarrelReadoutPhiEtaName = "BarHCal_Readout_phieta"
hcalExtBarrelReadoutPhiEtaName = "ExtBarHCal_Readout_phieta"
hcalEndcapReadoutName = "HECPhiEtaReco"
hcalFwdReadoutName = "HFwdPhiEta"

from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

num_events = 3
ApplicationMgr().EvtSel = 'NONE'
ApplicationMgr().EvtMax = num_events
ApplicationMgr().OutputLevel = DEBUG

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
# produced with Reconstruction/RecCalorimeter/tests/options/runFullCaloSystem_SimAndDigitisation.py
podioevent.input="http://fccsw.web.cern.ch/fccsw/testsamples/output_fullCalo_SimAndDigi_e50GeV_3events.root"
ApplicationMgr().ExtSvc += [podioevent]


# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput()
podioinput.collections = [
  ecalBarrelCellsName,
  ecalEndcapCellsName,
  ecalFwdCellsName,
  hcalBarrelCellsName,
  hcalExtBarrelCellsName,
  hcalEndcapCellsName,
  hcalFwdCellsName,
]
ApplicationMgr().TopAlg += [podioinput]

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc")
geoservice.detectors=[  
  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  #'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
  'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
  'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
  'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
  'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
  'file:Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml',
  ]
geoservice.OutputLevel = WARNING
ApplicationMgr().ExtSvc += [geoservice]


#Configure tools for calo cell positions
from Configurables import CellPositionsECalBarrelTool, CellPositionsHCalBarrelNoSegTool, CellPositionsHCalBarrelTool, CellPositionsCaloDiscsTool, CellPositionsTailCatcherTool
from Configurables import CellPositionsECalBarrelTool
ECalBcells = CellPositionsECalBarrelTool("CellPositionsECalBarrel")
ECalBcells.readoutName = ecalBarrelReadoutName
ECalBcells.OutputLevel = INFO

from Configurables import CellPositionsCaloDiscsTool
EMECcells = CellPositionsCaloDiscsTool("CellPositionsEMEC")
EMECcells.readoutName = ecalBarrelReadoutName

from Configurables import CellPositionsCaloDiscsTool
ECalFwdcells = CellPositionsCaloDiscsTool("CellPositionsECalFwd")
ECalFwdcells.readoutName = ecalFwdReadoutName
ECalFwdcells.OutputLevel = INFO

from Configurables import CellPositionsHCalBarrelNoSegTool
HCalBcellVols = CellPositionsHCalBarrelNoSegTool("CellPositionsHCalBarrelVols")
HCalBcellVols.readoutName = "HCalBarrelReadout"
HCalBcellVols.OutputLevel = INFO

from Configurables import CellPositionsHCalBarrelTool
HCalBsegcells = CellPositionsHCalBarrelTool("CellPositionsHCalBarrel")
HCalBsegcells.readoutName = "BarHCal_Readout_phieta"
HCalBsegcells.radii = [291.05, 301.05, 313.55, 328.55, 343.55, 358.55, 378.55, 413.55, 428.55, 453.55]
HCalBsegcells.OutputLevel = INFO

from Configurables import CellPositionsCaloDiscsTool
HECcells = CellPositionsCaloDiscsTool("CellPositionsHEC")
HECcells.readoutName = hcalEndcapReadoutName

from Configurables import CellPositionsCaloDiscsTool
HCalFwdcells = CellPositionsCaloDiscsTool("CellPositionsHCalFwd")
HCalFwdcells.readoutName = hcalFwdReadoutName


# additionally for HCal                                 
from Configurables import RewriteBitfield
# Use Phi-Eta segmentation in Hcal barrel               
rewriteHcal = RewriteBitfield("RewriteHCal")
# old bitfield (readout)
rewriteHcal.oldReadoutName = "HCalBarrelReadout"
# specify which fields are going to be deleted 
rewriteHcal.removeIds = ["row"]
# new bitfield (readout), with new segmentation
rewriteHcal.newReadoutName = "BarHCal_Readout_phieta"
rewriteHcal.debugPrint = 10
rewriteHcal.OutputLevel= INFO
# clusters are needed, with deposit position and cellID in bits
rewriteHcal.inhits.Path = "HCalBarrelCells"
rewriteHcal.outhits.Path = "newHCalBarrelCells"
ApplicationMgr().TopAlg += [rewriteHcal]

rewriteExtHcal = RewriteBitfield("RewriteExtHcal")
# old bitfield (readout)
rewriteExtHcal.oldReadoutName = "HCalExtBarrelReadout" #hcalExtBarrelReadoutName,
# specify which fields are going to be altered (deleted/rewritten)
rewriteExtHcal.removeIds = ["row"]
# new bitfield (readout), with new segmentation
rewriteExtHcal.newReadoutName = "ExtBarHCal_Readout_phieta"
rewriteExtHcal.debugPrint = 10
rewriteExtHcal.OutputLevel = INFO
rewriteExtHcal.inhits.Path = "HCalExtBarrelCells"
rewriteExtHcal.outhits.Path = "newHCalExtBarrelCells"
ApplicationMgr().TopAlg += [rewriteExtHcal]

##############################################################################################################
#######                                       REWRITE ENDCAP BITFIELD                            #############
##############################################################################################################

from Configurables import RewriteBitfield
rewriteECalEC = RewriteBitfield("RewriteECalEC")
# old bitfield (readout)
rewriteECalEC.oldReadoutName = "EMECPhiEta"
# specify which fields are going to be deleted
rewriteECalEC.removeIds = ["sublayer"]
# new bitfield (readout), with new segmentation
rewriteECalEC.newReadoutName = ecalEndcapReadoutName
rewriteECalEC.debugPrint = 10
rewriteECalEC.OutputLevel= INFO
# clusters are needed, with deposit position and cellID in bits
rewriteECalEC.inhits.Path = "ECalEndcapCells"
rewriteECalEC.outhits.Path = "newECalEndcapCells"
ApplicationMgr().TopAlg += [rewriteECalEC]

rewriteHCalEC = RewriteBitfield("RewriteHCalEC")
# old bitfield (readout)
rewriteHCalEC.oldReadoutName = "HECPhiEta"
# specify which fields are going to be deleted
rewriteHCalEC.removeIds = ["sublayer"]
# new bitfield (readout), with new segmentation
rewriteHCalEC.newReadoutName = hcalEndcapReadoutName
rewriteHCalEC.debugPrint = 10
rewriteHCalEC.OutputLevel = INFO
# clusters are needed, with deposit position and cellID in bits
rewriteHCalEC.inhits.Path = "HCalEndcapCells"
rewriteHCalEC.outhits.Path = "newHCalEndcapCells"
ApplicationMgr().TopAlg += [rewriteHCalEC]

# add noise, create all existing cells in detector
from Configurables import NoiseCaloCellsFromFileTool
noiseBarrel = NoiseCaloCellsFromFileTool("NoiseBarrel")
noiseBarrel.readoutName = ecalBarrelReadoutName
noiseBarrel.noiseFileName = "http://fccsw.web.cern.ch/fccsw/testsamples/elecNoise_ecalBarrel_50Ohm_traces2_2shieldWidth.root"
noiseBarrel.elecNoiseHistoName = "h_elecNoise_fcc_"
noiseBarrel.activeFieldName = "layer"
noiseBarrel.addPileup = False
noiseBarrel.cellPositionsTool = ECalBcells
noiseBarrel.numRadialLayers = 8

from Configurables import TubeLayerPhiEtaCaloTool
barrelGeometry = TubeLayerPhiEtaCaloTool("EcalBarrelGeo")
barrelGeometry.readoutName = ecalBarrelReadoutName
barrelGeometry.activeVolumeName = "LAr_sensitive"
barrelGeometry.activeFieldName = "layer"
barrelGeometry.fieldNames = ["system"]
barrelGeometry.fieldValues = [5]
barrelGeometry.activeVolumesNumber = 8

from Configurables import CreateCaloCells
createEcalBarrelCells = CreateCaloCells("CreateECalBarrelCells")
createEcalBarrelCells.geometryTool = barrelGeometry
createEcalBarrelCells.doCellCalibration=False # already calibrated
createEcalBarrelCells.addCellNoise=True
createEcalBarrelCells.filterCellNoise=False
createEcalBarrelCells.noiseTool = noiseBarrel
createEcalBarrelCells.hits=ecalBarrelCellsName
createEcalBarrelCells.cells=ecalBarrelCellsName+"Noise"
ApplicationMgr().TopAlg +=[createEcalBarrelCells]

# add noise, create all existing cells in detector
# currently only positive side!
from Configurables import NoiseCaloCellsFromFileTool
noiseEndcap = NoiseCaloCellsFromFileTool("NoiseEndcap")
noiseEndcap.readoutName = ecalEndcapReadoutName
noiseEndcap.noiseFileName = "http://fccsw.web.cern.ch/fccsw/testsamples/elecNoise_emec_50Ohm_2shieldWidth_6layers.root"
noiseEndcap.elecNoiseHistoName = "h_elecNoise_fcc_"
noiseEndcap.activeFieldName = "layer"
noiseEndcap.addPileup = False
noiseEndcap.cellPositionsTool = EMECcells
noiseEndcap.numRadialLayers = 6

from Configurables import TubeLayerPhiEtaCaloTool
endcapGeometry = TubeLayerPhiEtaCaloTool("EcalEndcapGeo")
endcapGeometry.readoutName = ecalEndcapReadoutName
endcapGeometry.activeVolumeName = "layerEnvelope"
endcapGeometry.activeFieldName = "layer"
endcapGeometry.activeVolumesNumber = 6
endcapGeometry.fieldNames = ["system"]
endcapGeometry.fieldValues = [6]

from Configurables import CreateCaloCells
createEcalEndcapCells = CreateCaloCells("CreateECalEndcapCells")
createEcalEndcapCells.geometryTool = endcapGeometry
createEcalEndcapCells.doCellCalibration=False # already calibrated
createEcalEndcapCells.addCellNoise=True
createEcalEndcapCells.filterCellNoise=False
createEcalEndcapCells.noiseTool = noiseEndcap
createEcalEndcapCells.hits="newECalEndcapCells"
createEcalEndcapCells.cells=ecalEndcapCellsName+"Noise"
ApplicationMgr().TopAlg +=[createEcalEndcapCells]

#Create calo clusters
from GaudiKernel.PhysicalConstants import pi

from Configurables import CaloTowerTool
towers = CaloTowerTool("towers")
towers.deltaEtaTower = 0.01
towers.deltaPhiTower = 2*pi/704.
towers.ecalBarrelReadoutName = ecalBarrelReadoutName
towers.ecalEndcapReadoutName = ecalEndcapReadoutName
towers.ecalFwdReadoutName = ecalFwdReadoutName
towers.hcalBarrelReadoutName = hcalBarrelReadoutPhiEtaName
towers.hcalExtBarrelReadoutName = hcalExtBarrelReadoutPhiEtaName
towers.hcalEndcapReadoutName = hcalEndcapReadoutName
towers.hcalFwdReadoutName = hcalFwdReadoutName

towers.ecalBarrelCells.Path = ecalBarrelCellsName + "Noise"
towers.ecalEndcapCells.Path = ecalEndcapCellsName + "Noise"
towers.ecalFwdCells.Path = ecalFwdCellsName
towers.hcalBarrelCells.Path = "newHCalBarrelCells"
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

from Configurables import CreateCaloClustersSlidingWindow
createClusters = CreateCaloClustersSlidingWindow("CreateClusters")
createClusters.towerTool = towers
createClusters.nEtaWindow = windE
createClusters.nPhiWindow = windP
createClusters.nEtaPosition = posE
createClusters.nPhiPosition = posP
createClusters.nEtaDuplicates = dupE
createClusters.nPhiDuplicates = dupP
createClusters.nEtaFinal = finE
createClusters.nPhiFinal = finP
createClusters.energyThreshold = threshold
createClusters.clusters.Path = "CaloClusters"
createClusters.clusterCells.Path = "CaloClusterCells"
createClusters.AuditExecute = True
ApplicationMgr().TopAlg +=[createClusters]

out = PodioOutput("out")
out.filename="output_runFullCaloSystem_ReconstructionSW_noiseFromFile.root"
out.outputCommands = ["keep *"]
out.AuditExecute = True
ApplicationMgr().TopAlg +=[out]


#ApplicationMgr(
#    TopAlg = [podioinput,
#              rewriteHcal,
#              rewriteExtHcal,
#              rewriteECalEC,
#              rewriteHCalEC,
#              createEcalBarrelCells,
#              createEcalEndcapCells,
#              createClusters,
#              out
#              ],
#    EvtSel = 'NONE',
#    EvtMax   = num_events,
#    ExtSvc = [podioevent, geoservice])
#
