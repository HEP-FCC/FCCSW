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

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detector='file:../DetectorDescription/Detectors/compact/ParametricSimTracker.xml',
                    OutputLevel = DEBUG)

from Configurables import G4SimSvc
geantservice = G4SimSvc("G4SimSvc", detector='DD4hepDetector', physicslist='G4FastSimPhysicsList', actions="G4FastSimActions")

from Configurables import GeantFastSimAlg, G4FastSimPhysicsList, G4FastSimActions, G4ParticleSmearSimple
geantsim = GeantFastSimAlg("GeantFastSimAlg")
geantsim.DataInputs.genParticles.Path="allGenParticles"
geantsim.DataOutputs.particles.Path = "recParticles"
geantsim.DataOutputs.particleassociation.Path = "particleMCparticleAssociation"

physicslist = G4FastSimPhysicsList("G4FastSimPhysicsList", fullphysics="G4FTFP_BERT")
geantservice.addTool(physicslist)

actions = G4FastSimActions("G4FastSimActions", smearing="G4ParticleSmearSimple")
geantservice.addTool(actions)
smear = G4ParticleSmearSimple("G4ParticleSmearSimple", sigma = 0.015)
actions.addTool(smear)

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
