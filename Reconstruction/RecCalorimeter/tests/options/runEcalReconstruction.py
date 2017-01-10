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

#from ecalCommonSetup import *

from Configurables import MergeLayers
mergelayers = MergeLayers("MergeLayers",
                   # take the bitfield description from the geometry service
                   readout = "ECalHitsPhiEta",
                   # cells in which field should be merged
                   identifier = "active_layer",
                   volumeName = "LAr_sensitive",
                   # how many cells to merge
                   # merge first 19 into new cell (id=0), next 71 into second cell (id=1), ...
                   merge = [19,71,9],
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
                              readoutName="ECalHitsPhiEta",
                              fieldNames=["system","ECAL_Cryo","bath","EM_barrel"],
                              fieldValues=[5,1,1,1],
                              activeVolumesNumber=3,
                              OutputLevel=INFO)
createcells.DataInputs.hits.Path="mergedECalHits"
createcells.DataOutputs.cells.Path="caloCells"

from Configurables import CreatePositionedHit
positionhit = CreatePositionedHit("CreatePositionedHit", readoutName = "ECalHitsPhiEta",activeFieldName = "active_layer",activeVolumeName="LAr_sensitive")
positionhit.DataInputs.caloCells.Path="caloCells"
positionhit.DataOutputs.caloPositionedHits.Path="caloCellsPositions"

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow
from GaudiKernel.PhysicalConstants import pi
createclusters = CreateCaloClustersSlidingWindow("CreateCaloClusters",
                                                 readoutName = "ECalHitsPhiEta",
                                                 fieldNames = ["system","ECAL_Cryo","bath","EM_barrel"],
                                                 fieldValues = [5,1,1,1],
                                                 deltaEtaTower = 0.01, deltaPhiTower = 2*pi/629.,
                                                 nEtaWindow = 5, nPhiWindow = 15,
                                                 nEtaPosition = 3, nPhiPosition = 3,
                                                 nEtaDuplicates = 5, nPhiDuplicates = 15,
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

