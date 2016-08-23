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
geoservice = GeoSvc("GeoSvc", detectors=['file:Test/TestGeometry/data/TestBoxCaloSD_3readouts.xml'], OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", physicslist='SimG4TestPhysicsList')

from Configurables import SimG4Alg, SimG4SaveCalHits, InspectHitsCollectionsTool
inspecttool = InspectHitsCollectionsTool("inspect", readoutNames=["ECalHits"], OutputLevel = DEBUG)
savecaltool = SimG4SaveCalHits("saveECalHits", readoutNames = ["ECalHits"], OutputLevel = DEBUG)
savecaltool.DataOutputs.caloClusters.Path = "caloClusters"
savecaltool.DataOutputs.caloHits.Path = "caloHits"
geantsim = SimG4Alg("SimG4Alg", outputs= ["SimG4SaveCalHits/saveECalHits","InspectHitsCollectionsTool/inspect"])

from Configurables import RedoSegmentation
resegment = RedoSegmentation("ReSegmentation",
                             # old bitfield (readout)
                             oldReadoutName = "ECalHits",
                             # specify which fields are going to be deleted
                             oldSegmentationIds = ["x","y","z"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName="ECalHitsReverseOrder",
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
