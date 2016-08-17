from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper

reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.7/testsamples/testHepMCrandom.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

dumper = HepMCDumper("Dumper")
dumper.DataInputs.hepmc.Path="hepmc"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Test/TestGeometry/data/TestBoxCaloSD_segmentation.xml'], OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", physicslist='SimG4TestPhysicsList')

from Configurables import SimG4Alg, SimG4SaveCalHits
savecaltool = SimG4SaveCalHits("saveECalHits", readoutNames = ["ECalHits"], OutputLevel = DEBUG)
savecaltool.DataOutputs.caloClusters.Path = "caloClusters"
savecaltool.DataOutputs.caloHits.Path = "caloHits"
geantsim = SimG4Alg("SimG4Alg", outputs= ["SimG4SaveCalHits/saveECalHits","InspectHitsCollectionsTool"])

from Configurables import RedoSegmentation, PhiEtaGridTool
from GaudiKernel.SystemOfUnits import mm
newgrid = PhiEtaGridTool("newSegm",
                               # new bitfield includes also detector fields
                               # (it will be checked if they agree with detector fields from the old readout - but not the size!)
                               bitfield = "phi:-8,eta:-8,system:1",
                               # cell sizes of the new segmentation
                               cellSizeEta = 0.1,
                               cellNumPhi = 100)
resegment = RedoSegmentation("ReSegmentation",
                             # specify the bitfield (e.g. if old geometry not available) ...
                             oldBitfield ="z:0:-4,y:4:-4,x:8:-4,system:12:1",
                             # ... but it is easier to take the old bitfield from the geometry service
                             # only one of two can be specified: either readoutName or oldBitfield
                             # readoutName = "ECalHits",
                             # specify which fields are going to be deleted
                             oldSegmentationIds = ["x","y","z"],
                             # the new segmentation tool
                             newSegmentation = newgrid,
                             OutputLevel = DEBUG)
# clusters are needed, with deposit position and cellID in bits
resegment.DataInputs.inclusters.Path = "caloClusters"
resegment.DataOutputs.outhits.Path = "newCaloHits"

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="testResegmentationRPhi.root")
out.outputCommands = ["keep *"]

ApplicationMgr(EvtSel='NONE',
               EvtMax=30,
               TopAlg=[reader, hepmc_converter, geantsim, resegment, out],
               ExtSvc = [podiosvc, geoservice, geantservice],
               OutputLevel=DEBUG)
