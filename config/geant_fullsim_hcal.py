from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

podioevent = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import GeantSvc
geantservice = GeantSvc("GeantSvc", config="GeantFullSimConfig", detector="DD4hepDetector")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:DetectorDescription/Detectors/compact/FCChh_DectMaster.xml',
                                         'file:DetectorDescription/Detectors/compact/FCChh_HCalBarrel.xml' ],
                    OutputLevel = DEBUG)

from Configurables import GeantFullSimAlg
geantsim = GeantFullSimAlg("GeantFullSimAlg")
geantsim.DataInputs.genParticles.Path="allGenParticles"
geantsim.DataOutputs.trackClusters.Path = "clusters"
geantsim.DataOutputs.trackHits.Path = "hits"
geantsim.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"
geantsim.DataOutputs.caloClusters.Path = "caloClusters"
geantsim.DataOutputs.caloHits.Path = "caloHits"

from Configurables import PodioWrite, PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [podioevent, geoservice, geantservice], # order! geo needed by geant
                OutputLevel=DEBUG
 )
