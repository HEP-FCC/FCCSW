

from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants

#### Data service
from Configurables import EventDataSvc
from k4FWCore import ApplicationMgr, IOSvc

ApplicationMgr().EvtSel = 'NONE' 
ApplicationMgr().EvtMax = 2
ApplicationMgr().OutputLevel = INFO


podioevent = EventDataSvc("EventDataSvc")
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

iosvc = IOSvc()
iosvc.outputCommands = ["keep *"]
iosvc.Output = "out_particle_gun.root"
