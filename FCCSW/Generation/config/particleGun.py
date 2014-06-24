from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCDumper, ParticleGunAlg, MomentumRangeParticleGun, Gaudi__ParticlePropertySvc

dumper = HepMCDumper("Dumper")
dumper.Inputs.hepmc.Path="hepmc"

particlePropertySvc = Gaudi__ParticlePropertySvc("ParticlePropertySvc")

guntool = MomentumRangeParticleGun()
gun = ParticleGunAlg("gun",ParticleGunTool = "MomentumRangeParticleGun")
gun.Outputs.hepmc.Path = "hepmc"

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[gun,dumper],
               SvcOptMapping = ["Gaudi::ParticlePropertySvc/ParticlePropertySvc"]
)

