from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

albersevent = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="../example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import GeantSvc
geantservice = GeantSvc("GeantSvc", detector="DD4hepDetector", physicslist="G4FTFP_BERT", actions="G4FullSimActions")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detector='file:../DetectorDescription/Detectors/compact/TestHCal.xml',
                    OutputLevel = DEBUG)

from Configurables import GeantFullSimAlg
geantsim = GeantFullSimAlg("GeantFullSimAlg")
geantsim.DataInputs.genParticles.Path="allGenParticles"
geantsim.DataOutputs.trackClusters.Path = "clusters"
geantsim.DataOutputs.trackHits.Path = "hits"
geantsim.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"
geantsim.DataOutputs.caloClusters.Path = "caloClusters"
geantsim.DataOutputs.caloHits.Path = "caloHits"

from Configurables import AlbersWrite, AlbersOutput
out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent, geoservice, geantservice], # order! geo needed by geant
                OutputLevel=INFO
 )
