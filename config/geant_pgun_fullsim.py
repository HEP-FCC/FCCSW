from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCDumper, FCCDataSvc

podioevent = FCCDataSvc("EventDataSvc")

# reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
# reader.DataOutputs.hepmc.Path = "hepmc"
from Configurables import ParticleGunAlg, MomentumRangeParticleGun, FlatSmearVertex#, Gaudi__ParticlePropertySvc
gen = ParticleGunAlg("ParticleGun", ParticleGunTool="MomentumRangeParticleGun", VertexSmearingTool="FlatSmearVertex")
gen.DataOutputs.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="/afs/cern.ch/lhcb/software/releases/GAUDI/GAUDI_v26r4/GaudiExamples/tests/data/ParticleTable.txt")

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:DetectorDescription/Detectors/compact/TestHCal.xml'],
                    OutputLevel = DEBUG)

from Configurables import GeantSvc
geantservice = GeantSvc("GeantSvc", config="GeantFullSimConfig", detector='DD4hepDetector')

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

ApplicationMgr( TopAlg = [gen, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [podioevent, geoservice, geantservice, ppservice], # order! geo needed by geant
                OutputLevel=DEBUG
 )
