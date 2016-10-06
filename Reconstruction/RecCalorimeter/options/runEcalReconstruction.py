from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", filename="/afs/cern.ch/exp/fcc/sw/0.7/testsamples/output_ecalSim_e50GeV_eta0_10events.root", collections=["ECalHits", "ECalClusters"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml'], 
                    OutputLevel = INFO)

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool
calibcells = CalibrateCaloHitsTool("CalibrateCaloHitsTool",invSamplingFraction="5.4")

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells", 
                              calibTool=calibcells, doCellCalibration=True,
                              addCellNoise=True,filterCellNoise=False,
                              readoutName="ECalHitsPhiEta",numVolumesRemove=1,fieldNames=["system","ECAL_Cryo","bath","EM_barrel"],fieldValues=[5,1,1,1],
                              OutputLevel=INFO)
createcells.DataInputs.hits.Path="ECalHits"
createcells.DataOutputs.cells.Path="caloCells"

from Configurables import CreatePositionedHit
positionhit = CreatePositionedHit("CreatePositionedHit", readoutName = "ECalHitsPhiEta",activeFieldName = "active_layer",activeVolumeName="LAr",numVolumesRemove="1")
positionhit.DataInputs.caloCells.Path="caloCells"
positionhit.DataOutputs.caloClusters.Path="caloCellsClusters"

out = PodioOutput("out", filename="output_ecalReco_test.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [podioinput,
              createcells,
              positionhit,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 10,
    ExtSvc = [podioevent, geoservice],
 )

