from Gaudi.Configuration import *
from Configurables import ApplicationMgr, THistSvc, Gaudi__ParticlePropertySvc
from Configurables import HepMCDumper, ParticleGunAlg, MomentumRangeParticleGun, HepMCHistograms

dumper = HepMCDumper("Dumper")
dumper.Inputs.hepmc.Path="hepmc"

particlePropertySvc = Gaudi__ParticlePropertySvc("ParticlePropertySvc")

guntool = MomentumRangeParticleGun()
gun = ParticleGunAlg("gun",ParticleGunTool = "MomentumRangeParticleGun")
gun.Outputs.hepmc.Path = "hepmc"

histo = HepMCHistograms("GenHistograms")
histo.Inputs.hepmc.Path="hepmc"

THistSvc().Output = ["rec DATAFILE='GenHistograms.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=VERBOSE

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[gun,dumper,histo],
               SvcOptMapping = ["Gaudi::ParticlePropertySvc/ParticlePropertySvc"]
)

