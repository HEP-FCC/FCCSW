from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE'
ApplicationMgr().EvtMax = 2
ApplicationMgr().OutputLevel = INFO

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]


from Configurables import MomentumRangeParticleGun
from GaudiKernel import PhysicalConstants as constants
guntool = MomentumRangeParticleGun()
guntool.ThetaMin = 0 
guntool.ThetaMax = 2 * constants.pi 
guntool.PdgCodes = [11]
from Configurables import GenAlg
gen = GenAlg()
gen.SignalProvider = guntool
gen.hepmc.Path = "hepmc"
ApplicationMgr().TopAlg += [gen]

# reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.GenParticles.Path="GenParticles"
ApplicationMgr().TopAlg += [hepmc_converter]


# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
path_to_detectors = os.environ.get("FCCDETECTORS", "")
geoservice = GeoSvc("GeoSvc")
detectors = [ 'Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
              'Detector/DetFCChhBaseline1/compact/FCChh_TrackerAir.xml',
              ]
geoservice.detectors = [ os.path.join(path_to_detectors, d) for d in detectors ]
ApplicationMgr().ExtSvc += [geoservice]

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import  SimG4UserLimitRegion
## create region and a parametrisation model, pass smearing tool
regiontool = SimG4UserLimitRegion("limits")
regiontool.volumeNames = ["TrackerEnvelopeBarrel"]
regiontool.maxStep = 1*mm

from Configurables import SimG4UserLimitPhysicsList
# create overlay on top of FTFP_BERT physics list, attaching fast sim/parametrization process
physicslisttool = SimG4UserLimitPhysicsList("Physics")
physicslisttool.fullphysics = "SimG4FtfpBert"

# attach those tools to the G4 service
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")
geantservice.physicslist = physicslisttool
geantservice.regions = [regiontool]
ApplicationMgr().ExtSvc += [geantservice]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
geantsim = SimG4Alg("SimG4Alg")
from Configurables import SimG4PrimariesFromEdmTool
geantsim.eventProvider = SimG4PrimariesFromEdmTool("EdmConverter")
geantsim.eventProvider.GenParticles.Path = "GenParticles"
ApplicationMgr().TopAlg += [geantsim]

#todo: add savetools?



# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out")
out.filename = "out_geant_userLimits.root"
out.outputCommands = ["keep *"]
out.OutputLevel = DEBUG
ApplicationMgr().TopAlg += [out]

