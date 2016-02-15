from Gaudi.Configuration import *
from Configurables import ApplicationMgr, THistSvc, Gaudi__ParticlePropertySvc
from Configurables import HepMCDumper, ParticleGunAlg, MomentumRangeParticleGun, HepMCHistograms, FlatSmearVertex

dumper = HepMCDumper("Dumper")
dumper.DataInputs.hepmc.Path="hepmc"



particlePropertySvc = Gaudi__ParticlePropertySvc("ParticlePropertySvc",
    ParticlePropertiesFile='Generation/data/ParticleTable.txt')
guntool = MomentumRangeParticleGun(PdgCodes=[-211, 211, -11, -13,  13, 11 ])

smeartool = FlatSmearVertex("smeartoolname")
# FCCSW standard unit of length: [mm]
smeartool.xVertexMin = -10
smeartool.xVertexMax = 10
smeartool.yVertexMin = -10
smeartool.yVertexMax = 10
smeartool.zVertexMin = -30
smeartool.zVertexMax = 30

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

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[gun,dumper, hepmc_converter,histo],
               OutputLevel=VERBOSE,
               SvcOptMapping = ["Gaudi::ParticlePropertySvc/ParticlePropertySvc"]
)

