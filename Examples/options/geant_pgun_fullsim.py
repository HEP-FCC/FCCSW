
### \file
### \ingroup SimulationExamples
### | **input (alg)**               | other algorithms                 |                                                         |                        |                                  | **output (alg)**                              |
### |-------------------------------|----------------------------------|---------------------------------------------------------|------------------------|----------------------------------|-----------------------------------------------|
### | generating single particle events from a given list of types, with momentum, phi and theta from a given range, saving to HepMC | convert `HepMC::GenEvent` to EDM | geometry parsed from XML (TestHCal.xml) by DD4hep using GeoSvc | FTFP_BERT physics list | saving HCal hits | write the EDM output to ROOT file using PODIO |



from Gaudi.Configuration import *

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import GenAlg, MomentumRangeParticleGun
## Particle Gun using MomentumRangeParticleGun tool and FlatSmearVertex
# MomentumRangeParticleGun generates particles of given type(s) within given momentum, phi and theta range
# FlatSmearVertex smears the vertex with uniform distribution
guntool = MomentumRangeParticleGun()
gen = GenAlg("ParticleGun", SignalProvider=guntool, VertexSmearingTool="FlatSmearVertex")
gen.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
## Particle service
# list of possible particles is defined in ParticlePropertiesFile
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="Generation/data/ParticleTable.txt")

from Configurables import HepMCToEDMConverter
## Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
## DD4hep geometry service
# Parses the given xml file
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                                         'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml',
                                         'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = DEBUG)

from Configurables import SimG4Svc
## Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions")

from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4SaveCalHits, SimG4PrimariesFromEdmTool
## Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"
# and a tool that saves the calorimeter hits with a name "SimG4SaveCalHits/saveCalHits"
savehcaltool = SimG4SaveCalHits("saveCalHits", readoutNames = ["ECalHitsPhiEta","BarHCal_Readout"])
savehcaltool.positionedCaloHits.Path = "positionedCaloHits"

savehcaltool.caloHits.Path = "caloHits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits", "SimG4SaveCalHits/saveCalHits"],
                    eventProvider=particle_converter)

from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[gen, hepmc_converter, geantsim, out],
                EvtSel='NONE',
                EvtMax=1,
                ## order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc=[podioevent, geoservice, geantservice, ppservice],
                OutputLevel=DEBUG
 )
