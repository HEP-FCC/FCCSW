
### \file
### \ingroup BasicExamples
### | **input (alg)**                                                                                                              | other algorithms                   |                                  |                                       | **output (alg)**                                |
### | ----------------------------------------------------                                                                         | ---------------------------------- | ---------------------            | ------------------------------------- | ----------------------------------------------- |
### | generate single particle events from a given list of types, with momentum, phi and theta from a given range, saving to HepMC | creating the histograms for HepMC   | convert `HepMC::GenEvent` to EDM | dump `HepMC::GenEvent`                | write the EDM output to ROOT file using PODIO   |

from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units


from Configurables import FCCDataSvc
## Data service

podioevent = FCCDataSvc("EventDataSvc")
from Configurables import ApplicationMgr, THistSvc, Gaudi__ParticlePropertySvc
from Configurables import HepMCDumper, MomentumRangeParticleGun, HepMCHistograms, FlatSmearVertex, ConstPileUp


particlePropertySvc = Gaudi__ParticlePropertySvc("ParticlePropertySvc",
    ParticlePropertiesFile='Generation/data/ParticleTable.txt')
guntool = MomentumRangeParticleGun("SignalProvider", PdgCodes=[-211])
guntool2 = MomentumRangeParticleGun("PileUpProvider", PdgCodes=[11 ])


smeartool = FlatSmearVertex("smeartoolname")
smeartool.xVertexMin = -10*units.mm
smeartool.xVertexMax = 10*units.mm
smeartool.yVertexMin = -10*units.mm
smeartool.yVertexMax = 10*units.mm
smeartool.zVertexMin = -30*units.mm
smeartool.zVertexMax = 30*units.mm

pileuptool = ConstPileUp("MyPileUpConfig", numPileUpEvents=0)

from Configurables import GenAlg
gun1 = GenAlg("gun1", SignalProvider=guntool )
gun2 = GenAlg("gun2", SignalProvider=guntool2)
gun1.hepmc.Path = "hepmc1"
gun2.hepmc.Path = "hepmc2"




from Configurables import HepMCConverter
hepmc_converter1 = HepMCConverter("Converter1")
hepmc_converter1.hepmc.Path="hepmc1"
hepmc_converter1.genparticles.Path="signalGenParticles"
hepmc_converter1.genvertices.Path="signalGenVertices"

hepmc_converter2 = HepMCConverter("Converter2")
hepmc_converter2.hepmc.Path="hepmc2"
hepmc_converter2.genparticles.Path="pileupGenParticles"
hepmc_converter2.genvertices.Path="pileupGenVertices"


from Configurables import GenMerge

mergealg = GenMerge()
mergealg.pileupGenParticles.Path="pileupGenParticles"
mergealg.pileupGenVertices.Path="pileupGenVertices"
mergealg.signalGenParticles.Path="signalGenParticles"
mergealg.signalGenVertices.Path="signalGenVertices"
mergealg.allGenParticles.Path="allGenParticles"
mergealg.allGenVertices.Path="allGenVertices"


from Configurables import PodioOutput
out = PodioOutput("out", filename = "output_gen.root")
out.outputCommands = ["keep *", "drop pileupGenVertices", "drop pileupGenParticles", "drop signalGenVertices", "drop signalGenParticles"]

ApplicationMgr(
               TopAlg=[gun1, gun2, hepmc_converter1, hepmc_converter2, mergealg, out],
               EvtSel='NONE',
               ExtSvc = [podioevent],
               EvtMax=1,
               OutputLevel=VERBOSE,
               SvcOptMapping = ["Gaudi::ParticlePropertySvc/ParticlePropertySvc"]
)

