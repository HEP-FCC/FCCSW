
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

dumper = HepMCDumper("Dumper")
dumper.hepmc.Path="hepmc"

from Configurables import HepMCFileWriter
writer = HepMCFileWriter("MyHepMCFileWriter")
writer.hepmc.Path="hepmc"


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

pileuptool = ConstPileUp("MyPileUpConfig", numPileUpEvents=1)

from Configurables import GenAlg
gun = GenAlg()
gun.hepmc.Path = "hepmc"




from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

histo = HepMCHistograms("GenHistograms")
histo.hepmc.Path="hepmc"

THistSvc().Output = ["rec DATAFILE='GenHistograms.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=VERBOSE

from Configurables import PodioOutput
out = PodioOutput("out", filename = "output_gen.root")
out.outputCommands = ["keep *"]

ApplicationMgr(
               TopAlg=[gun, dumper, writer, hepmc_converter, out],
               EvtSel='NONE',
               ExtSvc = [podioevent],
               EvtMax=1,
               OutputLevel=VERBOSE,
               SvcOptMapping = ["Gaudi::ParticlePropertySvc/ParticlePropertySvc"]
)

