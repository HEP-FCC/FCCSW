EVTMAX=100

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", filename="/afs/cern.ch/user/n/novaj/public/FCCSW_tests/output_ecal.root", collections=["ECalHits", "ECalClusters", "GenParticles", "GenVertices"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml'], 
                    OutputLevel = INFO)

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool
calibcells = CalibrateCaloHitsTool("CalibrateCaloHitsTool",invSamplingFraction="5.4")

from Configurables import CreateCaloCells
createcells = CreateCaloCells()
createcells.DataInputs.hits.Path="ECalHits"
createcells.DataOutputs.cells.Path="caloCells"
createcells.calibTool=calibcells
createcells.doCellCalibration=True
createcells.addCellNoise=True
createcells.filterCellNoise=False
#Readout details needed for adding noise to all calo cells
createcells.readoutName="ECalHitsPhiEta"
createcells.numVolumesRemove=1
createcells.fieldNames=["system","ECAL_Cryo","bath","EM_barrel"]
createcells.fieldValues=[5,1,1,1]
createcells.OutputLevel=INFO

from Configurables import CreatePositionedHit
positionhit = CreatePositionedHit()
positionhit.DataInputs.caloCells.Path="caloCells"
positionhit.DataOutputs.caloClusters.Path="caloCellsClusters"
positionhit.readoutName = "ECalHitsPhiEta"
positionhit.activeFieldName = "active_layer"
positionhit.activeVolumeName="LAr"
positionhit.numVolumesRemove="1"

out = PodioOutput("out", filename="output_ecalReco_test.root",
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [podioinput,
              createcells,
              positionhit,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = EVTMAX,
    ExtSvc = [podioevent, geoservice],
 )

