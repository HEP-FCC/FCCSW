from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# Particle Gun using MomentumRangeParticleGun tool and FlatSmearVertex
# MomentumRangeParticleGun generates particles of given type(s) within given momentum, phi and theta range
# FlatSmearVertex smears the vertex with uniform distribution
from Configurables import ParticleGunAlg
gen = ParticleGunAlg("ParticleGun", ParticleGunTool="MomentumRangeParticleGun", VertexSmearingTool="FlatSmearVertex")
gen.DataOutputs.hepmc.Path = "hepmc"

# Particle service
# list of possible particles is defined in ParticlePropertiesFile
from Configurables import Gaudi__ParticlePropertySvc
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="/afs/cern.ch/lhcb/software/releases/GAUDI/GAUDI_v26r4/GaudiExamples/tests/data/ParticleTable.txt")

# Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detector='file:DetectorDescription/Detectors/compact/TestHCal.xml',
                    OutputLevel = DEBUG)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import G4SimSvc
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist="G4FtfpBert",
                        actions="G4FullSimActions")

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import G4SimAlg, G4SaveTrackerHits, G4SaveCalHits
savetrackertool = G4SaveTrackerHits("G4SaveTrackerHits")
savetrackertool.DataOutputs.trackClusters.Path = "clusters"
savetrackertool.DataOutputs.trackHits.Path = "hits"
savetrackertool.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"
savehcaltool = G4SaveCalHits("G4SaveHCalHits", caloType = "HCal")
savehcaltool.DataOutputs.caloClusters.Path = "caloClusters"
savehcaltool.DataOutputs.caloHits.Path = "caloHits"
geantsim = G4SimAlg("G4SimAlg",
                        outputs= ["G4SaveTrackerHits/G4SaveTrackerHits", "G4SaveCalHits/G4SaveHCalHits"])
geantsim.DataInputs.genParticles.Path="allGenParticles"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [podioevent, geoservice, geantservice, ppservice], # order! geo needed by geant
                OutputLevel=DEBUG
 )
