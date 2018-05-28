import os
from Gaudi.Configuration import *

# geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'], OutputLevel = INFO)
# tracking geomery sercive
from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")


# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
####from Configurables import GenAlg, HepMCFileReader
####filereadertool = HepMCFileReader("HepMCFileReader", Filename="/eos/project/f/fccsw-web/testsamples/FCC_minbias_100TeV.dat")
####reader = GenAlg("Reader", SignalProvider=filereadertool)
####reader.hepmc.Path = "hepmc"

# reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
####from Configurables import HepMCToEDMConverter
####hepmc_converter = HepMCToEDMConverter("Converter")
####hepmc_converter.hepmc.Path="hepmc"
####hepmc_converter.genparticles.Path="allGenParticles"
####hepmc_converter.genvertices.Path="allGenVertices"

########
from Configurables import GenAlg,MomentumRangeParticleGun
pgun_tool = MomentumRangeParticleGun(PdgCodes=[13],ThetaMin=0.,ThetaMax=3.14, MomentumMin=10000000,MomentumMax=100000000)
reader = GenAlg("ParticleGun", SignalProvider=pgun_tool, VertexSmearingTool="FlatSmearVertex")
reader.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
# Particle service
# list of possible particles is defined in ParticlePropertiesFile
ppservice = Gaudi__ParticlePropertySvc(
                                       "ParticlePropertySvc", ParticlePropertiesFile="Generation/data/ParticleTable.txt")
from Configurables import HepMCToEDMConverter
## Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

########
# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"
savetrackertool.digiTrackHits.Path = "digiHits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits"],
                    eventProvider=particle_converter)


from Configurables import GeometricTrackerDigitizer
digitizer = GeometricTrackerDigitizer()
digitizer.digiTrackHitAssociation.Path="digiHits"
digitizer.trackClusters.Path="trackClusters"
digitizer.clusterTrackHits.Path="clusterTrackHits"
digitizer.planarClusters = "planarClusters"
digitizer.analogReadout=FALSE

from Configurables import SimpleClusterWriter
clusterWriter = SimpleClusterWriter()
clusterWriter.filename = "PlanarClusters.root"
clusterWriter.treename = "clusters"
clusterWriter.planarClusters = "planarClusters"


# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, digitizer, clusterWriter, out],
               EvtSel = 'NONE',
               EvtMax   = 1,
               # order is important, as GeoSvc is needed by SimG4Svc
               ExtSvc = [podioevent, geoservice, geantservice, trkgeoservice],
               OutputLevel=DEBUG,
               )
