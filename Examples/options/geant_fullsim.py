### | read events from a HepMC file | convert `HepMC::GenEvent` to EDM | geometry taken from GDML file (no sensitive detectors!) | FTFP_BERT physics list | empty action initialisation list | write the EDM output to ROOT file using PODIO |

import os

from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE' 
ApplicationMgr().EvtMax = 2
ApplicationMgr().OutputLevel = INFO
ApplicationMgr().ExtSvc = ['RndmGenSvc']

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]


from Configurables import MomentumRangeParticleGun
guntool = MomentumRangeParticleGun()
guntool.ThetaMin = 0 
guntool.ThetaMax = 2 * constants.pi 
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
hepmc_converter.GenParticles.Path="allGenParticles"
ApplicationMgr().TopAlg += [hepmc_converter]

# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
path_to_detectors = os.environ.get("FCCDETECTORS", "")
geoservice = GeoSvc("GeoSvc")
detectors = [ 'Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
              'Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
              ]
geoservice.detectors = [ os.path.join(path_to_detectors, d) for d in detectors ]
geoservice.OutputLevel = INFO 
ApplicationMgr().ExtSvc += [geoservice]

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc")
geantservice.detector =     "SimG4DD4hepDetector"
geantservice.physicslist =  "SimG4FtfpBert"
geantservice.actions =      "SimG4FullSimActions"
ApplicationMgr().ExtSvc += [geantservice]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg
geantsim = SimG4Alg("SimG4Alg")
from Configurables import SimG4SaveTrackerHits
savetrackerbarreltool = SimG4SaveTrackerHits("saveTrackerHitsBarrel")
savetrackerbarreltool.readoutName = "TrackerBarrelReadout"
savetrackerbarreltool.SimTrackHits.Path = "TrackerBarrelReadout"
geantsim.outputs = [savetrackerbarreltool]
savetrackerendcaptool = SimG4SaveTrackerHits("saveTrackerHitsEndcap")
savetrackerendcaptool.readoutName = "TrackerEndcapReadout"
savetrackerendcaptool.SimTrackHits.Path = "TrackerEndcapReadout"
geantsim.outputs += [savetrackerendcaptool]
from Configurables import SimG4PrimariesFromEdmTool
geantsim.eventProvider = SimG4PrimariesFromEdmTool("EdmConverter")
geantsim.eventProvider.GenParticles.Path = "allGenParticles"
ApplicationMgr().TopAlg += [geantsim]

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput()
out.outputCommands = ["keep *"]
out.filename = "out_geant_fullsim.root"
ApplicationMgr().TopAlg += [out]

