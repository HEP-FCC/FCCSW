from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

albersevent = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="all_genparticles"
hepmc_converter.DataOutputs.genvertices.Path="all_genvertices"

from Configurables import sim__GeantSvc
geantservice = sim__GeantSvc("GeantSvc", config="GeantFastSimConfig")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detector='file:DetectorDescription/Detectors/compact/ParametricSimTracker.xml', OutputLevel = DEBUG)

from Configurables import sim__GeantFastSimAlg, GeantFastSimConfig, SimpleSmear
geantsim = sim__GeantFastSimAlg("GeantFastSimAlg")
geantsim.DataInputs.genParticles.Path="all_genparticles"
geantsim.DataOutputs.particles.Path = "recparticles"
geantsim.DataOutputs.particleassociation.Path = "particleMCparticle"
fastsimconfig = GeantFastSimConfig("FastSimConfig", smearing = "SimpleSmear")
geantsim.addTool(fastsimconfig)
smear = SimpleSmear("SimpleSmear", sigma = 0.015)
fastsimconfig.addTool(smear)

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
