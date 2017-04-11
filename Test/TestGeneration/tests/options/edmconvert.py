from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units

from Configurables import FCCDataSvc
## Data service

podioevent = FCCDataSvc("EventDataSvc")
from Configurables import ApplicationMgr, Gaudi__ParticlePropertySvc
from Configurables import HepMCDumper, MomentumRangeParticleGun, HepMCHistograms, FlatSmearVertex, ConstPileUp




particlePropertySvc = Gaudi__ParticlePropertySvc("ParticlePropertySvc",
    ParticlePropertiesFile='Generation/data/ParticleTable.txt')
guntool = MomentumRangeParticleGun("SignalProvider", PdgCodes=[11])




from Configurables import GenAlg
gun = GenAlg(SignalProvider=guntool)
gun.hepmc.Path = "hepmc"




from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"


# convert (final state) EDM back to HepMC
from Configurables import EDMToHepMCConverter
edm_converter = EDMToHepMCConverter("BackConverter")
edm_converter.hepmc.Path="hepmcconverted"
edm_converter.genparticles.Path="allGenParticles"
edm_converter.genvertices.Path="allGenVertices"
from Configurables import HepMCDumper
dumper = HepMCDumper("Dumper")
dumper.hepmc.Path="hepmc"

from Configurables import PodioOutput
out = PodioOutput("out", filename = "output_gen.root")
out.outputCommands = ["keep *"]

ApplicationMgr(
               TopAlg=[gun, hepmc_converter, edm_converter, dumper, out],
               EvtSel='NONE',
               ExtSvc = [podioevent],
               EvtMax=1,
               OutputLevel=VERBOSE,
               SvcOptMapping = ["Gaudi::ParticlePropertySvc/ParticlePropertySvc"]
)

