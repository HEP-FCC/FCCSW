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

from Configurables import G4SimSvc, G4FastSimPhysicsList, G4FastSimActions, G4ParticleSmearSimple
smeartool = G4ParticleSmearSimple("Smear", sigma = 0.15)
actionstool = G4FastSimActions("Actions", smearing=smeartool)
physicslisttool = G4FastSimPhysicsList("Physics", fullphysics="G4FtfpBert")
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist=physicslisttool, actions=actionstool)

from Configurables import G4FastSimAlg, G4SaveSmearedParticles
saveparticlestool = G4SaveSmearedParticles("G4SaveSmearedParticles")
saveparticlestool.DataOutputs.particles.Path = "smearedParticles"
saveparticlestool.DataOutputs.particlesMCparticles.Path = "particleMCparticleAssociation"
geantsim = G4FastSimAlg("G4Alg", outputs = ["G4SaveSmearedParticles/G4SaveSmearedParticles"])
geantsim.DataInputs.genParticles.Path="allGenParticles"


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
