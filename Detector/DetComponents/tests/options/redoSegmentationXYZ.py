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

from Configurables import RedoSegmentation, CartesianGridXYZTool
from GaudiKernel.SystemOfUnits import mm
newgrid = CartesianGridXYZTool("newSegm",
                               # new bitfield includes also detector fields
                               # (it will be checked if they agree with detector fields from the old readout - but not the size!)
                               bitfield = "system:0:1,z:1:-4,y:5:-4,x:9:-4",
                               # cell sizes of the new segmentation
                               # in this example the cells do not change, but bitfield is reschuffled
                               cellSizeX = 5*mm,
                               cellSizeY = 5*mm,
                               cellSizeZ = 5*mm)
resegment = RedoSegmentation("ReSegmentation",
                             # take the old bitfield from the geometry service...
                             readoutName = "ECalHits",
                             # ... but instead the bitfield can be defined as string (if old geometry not available)
                             # only one of two can be specified: either readoutName or oldBitfield
                             # oldBitfield ="z:0:-4,y:4:-4,x:8:-4,system:12:1",
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
out = PodioOutput("out", filename="testResegmentationXYZ.root")
out.outputCommands = ["keep *"]

ApplicationMgr(EvtSel='NONE',
               EvtMax=30,
               TopAlg=[reader, hepmc_converter, geantsim, resegment, out],
               ExtSvc = [podiosvc, geoservice, geantservice],
               OutputLevel=DEBUG)
