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
from Configurables import CalibrateCaloHitsTool, NoiseCaloCellsFromFileTool, TubeLayerPhiEtaCaloTool
calibcells = CalibrateCaloHitsTool("CalibrateCaloHitsTool", invSamplingFraction="5.4")
noise = NoiseCaloCellsFromFileTool("NoiseCaloCellsFromFileTool")
ecalgeo = TubeLayerPhiEtaCaloTool("EcalGeo",
                                  readoutName=ecalReadoutName,
                                  activeVolumeName = ecalVolumeName,
                                  activeFieldName = ecalIdentifierName,
                                  fieldNames=ecalFieldNames,
                                  fieldValues=ecalFieldValues,
                                  # to make it working with MergeLayers algorithm
                                  activeVolumesNumber=ecalNumberOfLayers,
                                  OutputLevel=DEBUG)

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              geometryTool = ecalgeo,
                              doCellCalibration=True,
                              calibTool=calibcells,
                              addCellNoise=True, filterCellNoise=False,
                              noiseTool=noise,
                              OutputLevel=DEBUG)
createcells.DataInputs.hits.Path="mergedECalHits"
createcells.DataOutputs.cells.Path="caloCells"

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow
from GaudiKernel.PhysicalConstants import pi
createclusters = CreateCaloClustersSlidingWindow("CreateCaloClusters",
                                                 readoutName = ecalReadoutName,
                                                 fieldNames = ecalFieldNames,
                                                 fieldValues = ecalFieldValues,
                                                 deltaEtaTower = 0.01, deltaPhiTower = 2*pi/629.,
                                                 nEtaWindow = 5, nPhiWindow = 15,
                                                 nEtaPosition = 5, nPhiPosition = 5,
                                                 nEtaDuplicates = 5, nPhiDuplicates = 15,
                                                 nEtaFinal = 5, nPhiFinal = 15,
                                                 energyThreshold = 10,
                                                 OutputLevel = DEBUG)
createclusters.DataInputs.cells.Path="caloCells"
createclusters.DataOutputs.clusters.Path="caloClusters"

out = PodioOutput("out", filename="output_ecalReco_noiseFromFile_test.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createclusters.AuditExecute = True
createcells.AuditExecute = True
mergelayers.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              mergelayers,
              createcells,
              createclusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 10,
    ExtSvc = [podioevent, geoservice],
 )

