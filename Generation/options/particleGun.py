from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units

from Configurables import ApplicationMgr
ApplicationMgr(
               EvtSel='NONE',
               EvtMax=1,
               OutputLevel=VERBOSE,
              )

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]



from Configurables import ConstPtParticleGun
guntool1 = ConstPtParticleGun("SignalProvider", PdgCodes=[-211])
guntool2 = ConstPtParticleGun("PileUpProvider", PdgCodes=[11], writeParticleGunBranches=False)
from Configurables import FlatSmearVertex
smeartool = FlatSmearVertex()
smeartool.xVertexMin = -10*units.mm
smeartool.xVertexMax = 10*units.mm
smeartool.yVertexMin = -10*units.mm
smeartool.yVertexMax = 10*units.mm
smeartool.zVertexMin = -30*units.mm
smeartool.zVertexMax = 30*units.mm
from Configurables import RangePileUp
pileuptool = RangePileUp()
pileuptool.numPileUpEvents=[1, 2, 3]
from Configurables import GenAlg
gun = GenAlg()
gun.SignalProvider = guntool1
gun.PileUpProvider = guntool2
gun.PileUpTool = pileuptool
gun.hepmc.Path = "hepmc"
ApplicationMgr().TopAlg += [gun]

from Configurables import HepMCDumper
dumper = HepMCDumper()
dumper.hepmc.Path="hepmc"
ApplicationMgr().TopAlg += [dumper]

from Configurables import HepMCFileWriter
writer = HepMCFileWriter()
writer.hepmc.Path="hepmc"
ApplicationMgr().TopAlg += [writer]

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter()
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path = "GenParticles"
hepmc_converter.genvertices.Path = "GenVertices"
ApplicationMgr().TopAlg += [hepmc_converter]

from Configurables import HepMCHistograms
histo = HepMCHistograms("GenHistograms")
histo.hepmc.Path="hepmc"
ApplicationMgr().TopAlg += [histo]

from Configurables import THistSvc
THistSvc().Output = ["rec DATAFILE='output_particleGun_GenHistograms.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=VERBOSE

from Configurables import PodioOutput
out = PodioOutput("out", filename = "output_particleGun.root")
out.outputCommands = ["keep *"]



