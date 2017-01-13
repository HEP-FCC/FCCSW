from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input="output_hcalSim_pi50GeV_eta036_10events.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["HCalHits", "HCalPositionedHits"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool
calibcells = CalibrateCaloHitsTool("CalibrateCaloHitsTool", invSamplingFraction="40")

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              calibTool=calibcells, doCellCalibration=True,
                              addCellNoise=True, filterCellNoise=False,

                              readoutName="BarHCal_Readout",
                              fieldNames=["system"],
                              fieldValues=[6],
                              OutputLevel=INFO)
createcells.DataInputs.hits.Path="HCalHits"
createcells.DataOutputs.cells.Path="caloCells"

from Configurables import CreatePositionedHit
positionhit = CreatePositionedHit("CreatePositionedHit", readoutName = "HCalHitsPhiEta",activeFieldName = "active_layer",activeVolumeName="LAr_sensitive")
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
                                                 nEtaWindow = 9, nPhiWindow = 9,
                                                 nEtaPosition = 7, nPhiPosition = 7,
                                                 nEtaDuplicates = 9, nPhiDuplicates = 9,
                                                 energyThreshold = 3,
                                                 OutputLevel = DEBUG)
createclusters.DataInputs.cells.Path="caloCells"
createclusters.DataOutputs.clusters.Path="caloClusters"

out = PodioOutput("out", filename="output_hcalReco_test.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [podioinput,
              createcells,
#              positionhit,
#              createclusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 10,
    ExtSvc = [podioevent, geoservice],
 )

