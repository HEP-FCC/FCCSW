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
from Configurables import PrepareEmptyPhiEtaRCaloCellsTool, MergeHitsToCaloCellsTool, CalibrateCaloCellsTool
emptycells = PrepareEmptyPhiEtaRCaloCellsTool("PrepareEmptyPhiEtaRCaloCellsTool",readoutName="ECalHitsPhiEta", activeFieldName="active_layer",
                                       activeVolumeName="LAr", numVolumesRemove="1",
                                       fieldNames=["system","ECAL_Cryo","bath","EM_barrel"], fieldValues=[5,1,1,1])
mergehits = MergeHitsToCaloCellsTool("MergeHitsToCaloCellsTool",readoutName="ECalHitsPhiEta")
calibcells = CalibrateCaloCellsTool("CalibrateCaloCellsTool",invSamplingFraction="5.4")

from Configurables import CreateCaloCells
createcells = CreateCaloCells()
createcells.DataInputs.hits.Path="ECalHits"
createcells.DataOutputs.cells.Path="caloCells"
createcells.prepareCellsTool=emptycells
createcells.mergeTool=mergehits
createcells.calibTool=calibcells
createcells.doCellCalibration=True
createcells.addCellNoise=False
createcells.OutputLevel=INFO

from Configurables import SavePhiEtaRCaloCellsToClusters
saveclusters = SavePhiEtaRCaloCellsToClusters()
saveclusters.DataInputs.caloCells.Path="caloCells"
saveclusters.DataOutputs.caloClusters.Path="caloCellsClusters"
saveclusters.readoutName = "ECalHitsPhiEta"
saveclusters.activeFieldName = "active_layer"
saveclusters.activeVolumeName="LAr"
saveclusters.numVolumesRemove="1"

out = PodioOutput("out", filename="output_ecalReco_test.root",
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [podioinput,
#              resegment,
              createcells,
              saveclusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = EVTMAX,
    ExtSvc = [podioevent, geoservice],
 )

