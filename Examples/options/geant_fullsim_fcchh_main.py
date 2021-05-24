
### | read events from a HepMC file | convert `HepMC::GenEvent` to EDM | geometry taken from GDML file (no sensitive detectors!) | FTFP_BERT physics list | empty action initialisation list | write the EDM output to ROOT file using PODIO |



from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants

from detector_fcchh_main import *


from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE' 
ApplicationMgr().EvtMax = 2
ApplicationMgr().OutputLevel = INFO
ApplicationMgr().ExtSvc += ['RndmGenSvc']

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]


from p0001_particle_gun_single_electron import *


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

