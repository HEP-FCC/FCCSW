from Gaudi.Configuration import *
from Configurables import ApplicationMgr, THistSvc, Gaudi__ParticlePropertySvc
from Configurables import HepMCDumper, ParticleGunAlg, MomentumRangeParticleGun, HepMCHistograms, FlatSmearVertex

dumper = HepMCDumper("Dumper")
dumper.DataInputs.hepmc.Path="hepmc"


# A particle table file needs to be present in the directory 
# from which Gaudi is run. If you run from the top-level directory,
# the following option can be added to the service to use the file in this directory.
#ParticlePropertiesFile='Generation/options/ParticleTable.txt'

particlePropertySvc = Gaudi__ParticlePropertySvc("ParticlePropertySvc")
guntool = MomentumRangeParticleGun()

gun = ParticleGunAlg("gun", ParticleGunTool = "MomentumRangeParticleGun", VertexSmearingTool = "FlatSmearVertex" )
gun.DataOutputs.hepmc.Path = "hepmc"

gun.addTool(FlatSmearVertex, name="FlatSmearVertex")

gun.FlatSmearVertex.xVertexMin = -10
gun.FlatSmearVertex.xVertexMax = 10
gun.FlatSmearVertex.yVertexMin = -10
gun.FlatSmearVertex.yVertexMax = 10
gun.FlatSmearVertex.zVertexMin = -30
gun.FlatSmearVertex.zVertexMax = 30

histo = HepMCHistograms("GenHistograms")
histo.DataInputs.hepmc.Path="hepmc"

THistSvc().Output = ["rec DATAFILE='GenHistograms.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=VERBOSE

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[gun,dumper,histo],
               OutputLevel=VERBOSE,
               SvcOptMapping = ["Gaudi::ParticlePropertySvc/ParticlePropertySvc"]
)

