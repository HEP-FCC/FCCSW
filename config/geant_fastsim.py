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

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detector='file:DetectorDescription/Detectors/compact/ParametricSimTracker.xml',
                    OutputLevel = DEBUG)

from Configurables import GeantSvc
geantservice = GeantSvc("GeantSvc", config="GeantFastSimConfig", detector='DD4hepDetector')

from Configurables import GeantFastSimAlg, GeantFastSimConfig, SimpleSmear
geantsim = GeantFastSimAlg("GeantFastSimAlg")
geantsim.DataInputs.genParticles.Path="allGenParticles"
geantsim.DataOutputs.particles.Path = "recParticles"
geantsim.DataOutputs.particleassociation.Path = "particleMCparticleAssociation"
fastsimconfig = GeantFastSimConfig("FastSimConfig", smearing = "SimpleSmear")
geantservice.addTool(fastsimconfig)
smear = SimpleSmear("SimpleSmear", sigma = 0.015)
fastsimconfig.addTool(smear)

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
