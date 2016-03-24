
### \file
### \ingroup BasicExamples
### | **input (alg)**                                                                                                              | other algorithms                   |                                  |                                       | **output (alg)**                                |
### | ----------------------------------------------------                                                                         | ---------------------------------- | ---------------------            | ------------------------------------- | ----------------------------------------------- |
### | generate single particle events from a given list of types, with momentum, phi and theta from a given range, saving to HepMC | creating the histograms for HepMC   | convert `HepMC::GenEvent` to EDM | dump `HepMC::GenEvent`                | write the EDM output to ROOT file using PODIO   |

from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units

from Configurables import ApplicationMgr, THistSvc, Gaudi__ParticlePropertySvc
from Configurables import HepMCDumper, ParticleGunAlg, MomentumRangeParticleGun, HepMCHistograms, FlatSmearVertex

dumper = HepMCDumper("Dumper")
dumper.DataInputs.hepmc.Path="hepmc"



particlePropertySvc = Gaudi__ParticlePropertySvc("ParticlePropertySvc",
    ParticlePropertiesFile='Generation/data/ParticleTable.txt')
guntool = MomentumRangeParticleGun(PdgCodes=[-211, 211, -11, -13,  13, 11 ])

smeartool = FlatSmearVertex("smeartoolname")
smeartool.xVertexMin = -10*units.mm
smeartool.xVertexMax = 10*units.mm
smeartool.yVertexMin = -10*units.mm
smeartool.yVertexMax = 10*units.mm
smeartool.zVertexMin = -30*units.mm
smeartool.zVertexMax = 30*units.mm

gun = ParticleGunAlg("gun", ParticleGunTool = guntool, VertexSmearingToolPGun=smeartool)
gun.DataOutputs.hepmc.Path = "hepmc"




from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

histo = HepMCHistograms("GenHistograms")
histo.DataInputs.hepmc.Path="hepmc"

THistSvc().Output = ["rec DATAFILE='GenHistograms.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=VERBOSE

ApplicationMgr(
               TopAlg=[gun, dumper, hepmc_converter, histo],
               EvtSel='NONE',
               EvtMax=1,
               OutputLevel=VERBOSE,
               SvcOptMapping = ["Gaudi::ParticlePropertySvc/ParticlePropertySvc"]
)

