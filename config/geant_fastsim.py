from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

albersevent = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detector='file:DetectorDescription/Detectors/compact/ParametricSimTracker.xml',
                    OutputLevel = DEBUG)

from Configurables import GeantSvc, G4FastSimPhysicsList, G4FastSimActions, SimpleSmear
smeartool = SimpleSmear("Smear", sigma = 0.015)
actionstool = G4FastSimActions("Actions", smearing=smeartool)
physicslisttool = G4FastSimPhysicsList("Physics", fullphysics="G4FTFP_BERT")
geantservice = GeantSvc("GeantSvc", detector='DD4hepDetector', physicslist=physicslisttool, actions=actionstool)

from Configurables import GeantFastSimAlg
geantsim = GeantFastSimAlg("G4Alg")
geantsim.DataInputs.genParticles.Path="allGenParticles"
geantsim.DataOutputs.particles.Path = "recParticles"
geantsim.DataOutputs.particleassociation.Path = "particleMCparticleAssociation"


from Configurables import AlbersWrite, AlbersOutput
out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent, geoservice, geantservice], # order! geo needed by geant
                OutputLevel=DEBUG
 )
