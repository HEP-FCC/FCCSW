EVTMAX=100

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", filename="output_ecal.root", collections=["ECalHits", "ECalClusters", "GenParticles", "GenVertices"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml'], 
                    OutputLevel = DEBUG)

#from Configurables import RedoSegmentation
#resegment = RedoSegmentation("ReSegmentation",
                             # old bitfield (readout)
#                             oldReadoutName = "ECalHitsPhiEta",
                             # specify which fields are going to be deleted
#                             oldSegmentationIds = ["phi","eta"],
                             # new bitfield (readout), with new segmentation
#                            newReadoutName = "ECalHitsPhiEtaNew",
#                             OutputLevel = INFO)
# clusters are needed, with deposit position and cellID in bits
#resegment.DataInputs.inclusters.Path = "ECalClusters"
#resegment.DataOutputs.outhits.Path = "ECalHitsResegment"



from Configurables import PrepareEmptyCaloCellsTool
emptycells = PrepareEmptyCaloCellsTool("PrepareEmptyCaloCellsTool",readoutName="ECalHitsPhiEta", fieldName="active_layer")

from Configurables import CreateCaloCells
createcells = CreateCaloCells()
createcells.DataInputs.hits.Path="ECalHits"
createcells.DataOutputs.caloCells.Path="caloCells"
createcells.prepareTool=emptycells
createcells.doCellCalibration=True
createcells.addCellNoise=False
createcells.OutputLevel=INFO

from Configurables import SaveCaloClusters
saveclusters = SaveCaloClusters()
saveclusters.DataInputs.caloCells.Path="caloCells"
saveclusters.DataOutputs.caloClusters.Path="caloCellsClusters"
saveclusters.readoutName = "ECalHitsPhiEta"
saveclusters.fieldName = "active_layer"


out = PodioOutput("out", filename="output_recoPhiEta_noise_test.root",
                   OutputLevel=INFO)
#out.outputCommands = ["keep *", "drop ECalHits", "drop ECalClusters"]
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

