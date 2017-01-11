from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input="output_ecalSim_e50GeV_eta0_10events.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["ECalHits", "ECalPositionedHits"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml'],
                    OutputLevel = INFO)

# common ECAL specific information
# readout name
ecalReadoutName = "ECalHitsPhiEta"
# active material identifier name
ecalIdentifierName = "active_layer"
# active material volume name
ecalVolumeName = "LAr_sensitive"
# number of active layers to be merged to create cells
ecalNumberOfLayersToMerge = [19,71,9]
# number of ECAL layers
ecalNumberOfLayers = len(ecalNumberOfLayersToMerge)
# ECAL bitfield names & values
ecalFieldNames=["system","ECAL_Cryo","bath","EM_barrel"]
ecalFieldValues=[5,1,1,1]

from Configurables import MergeLayers
mergelayers = MergeLayers("MergeLayers",
                   # take the bitfield description from the geometry service
                   readout = ecalReadoutName,
                   # cells in which field should be merged
                   identifier = ecalIdentifierName,
                   volumeName = ecalVolumeName,
                   # how many cells to merge
                   # merge first 19 into new cell (id=0), next 71 into second cell (id=1), ...
                   merge = ecalNumberOfLayersToMerge,
                   OutputLevel = INFO)
mergelayers.DataInputs.inhits.Path = "ECalHits"
mergelayers.DataOutputs.outhits.Path = "mergedECalHits"

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool
calibcells = CalibrateCaloHitsTool("CalibrateCaloHitsTool", invSamplingFraction="5.4")

from Configurables import NoiseCaloCellsFromFileTool
noisefile = NoiseCaloCellsFromFileTool("NoiseCaloCellsFromFileTool")

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              calibTool=calibcells, doCellCalibration=True,
                              noiseTool=noisefile,
                              addCellNoise=True, filterCellNoise=False,
                              useVolumeIdOnly=False,
                              readoutName=ecalReadoutName,
                              fieldNames=ecalFieldNames,
                              fieldValues=ecalFieldValues,
                              activeVolumesNumber=ecalNumberOfLayers,
                              OutputLevel=INFO)
createcells.DataInputs.hits.Path="mergedECalHits"
createcells.DataOutputs.cells.Path="caloCells"

from Configurables import CreatePositionedHit
positionhit = CreatePositionedHit("CreatePositionedHit", readoutName = ecalReadoutName,activeFieldName = ecalIdentifierName,activeVolumeName=ecalVolumeName)
positionhit.DataInputs.caloCells.Path="caloCells"
positionhit.DataOutputs.caloPositionedHits.Path="caloCellsPositions"

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow
from GaudiKernel.PhysicalConstants import pi
createclusters = CreateCaloClustersSlidingWindow("CreateCaloClusters",
                                                 readoutName = ecalReadoutName,
                                                 fieldNames = ecalFieldNames,
                                                 fieldValues = ecalFieldValues,
                                                 deltaEtaTower = 0.01, deltaPhiTower = 2*pi/629.,
                                                 nEtaWindow = 5, nPhiWindow = 15,
                                                 nEtaPosition = 3, nPhiPosition = 3,
                                                 nEtaDuplicates = 5, nPhiDuplicates = 15,
                                                 nEtaFinal = 5, nPhiFinal = 15,
                                                 energyThreshold = 7,
                                                 OutputLevel = DEBUG)
createclusters.DataInputs.cells.Path="caloCells"
createclusters.DataOutputs.clusters.Path="caloClusters"

out = PodioOutput("out", filename="output_ecalReco_test.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [podioinput,
              mergelayers,
              createcells,
              positionhit,
              createclusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 10,
    ExtSvc = [podioevent, geoservice],
 )

