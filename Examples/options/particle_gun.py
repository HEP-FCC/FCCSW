

from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE' 
ApplicationMgr().EvtMax = 2
ApplicationMgr().OutputLevel = INFO

#### Data service
from Configurables import k4DataSvc
podioevent = k4DataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]

from Configurables import MomentumRangeParticleGun
guntool = MomentumRangeParticleGun()
guntool.ThetaMin = 0 
guntool.ThetaMax = 2 * constants.pi 
guntool.MomentumMin = 100*units.GeV
guntool.MomentumMax = 100*units.GeV
guntool.PdgCodes = [11]

from Configurables import GenAlg
gen = GenAlg()
gen.SignalProvider=guntool
gen.hepmc.Path = "hepmc"
ApplicationMgr().TopAlg += [gen]

from Configurables import HepMCToEDMConverter
## reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.GenParticles.Path="GenParticles"
ApplicationMgr().TopAlg += [hepmc_converter]

from Configurables import PodioOutput
out = PodioOutput("out")
out.outputCommands = ["keep *"]
out.filename = "out_particle_gun.root"
ApplicationMgr().TopAlg += [out]
