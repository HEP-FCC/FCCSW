

from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")

## create DD4hep geometry
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'],
                                         OutputLevel=INFO)
## create tracking geometry
from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool(
    "SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4")

from Configurables import ConstPtParticleGun
pgun_tool = ConstPtParticleGun(PdgCodes=[13], EtaMin=0.0, EtaMax=6.0, PhiMin=0.0, PhiMax=0.0, PtMin=10000, PtMax=10000)

from Configurables import GenAlg
gen = GenAlg("ParticleGun", SignalProvider=pgun_tool, VertexSmearingTool="FlatSmearVertex")
gen.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
## Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"


## configure the extrapolation tool
from Configurables import ActsExtrapolationTool
extrapolationTool = ActsExtrapolationTool("TrackExtrapolationTool")
extrapolationTool.trackingGeometrySvc   = trkgeoservice
extrapolationTool.collectSensitive      = TRUE
extrapolationTool.collectPassive        = TRUE
extrapolationTool.collectBoundary       = TRUE
extrapolationTool.collectMaterial       = TRUE
extrapolationTool.sensitiveCurvilinear  = FALSE
extrapolationTool.searchMode            = 1
extrapolationTool.pathLimit             = -1.
extrapolationTool.bFieldZ = 4.

## configure the extrapolation test
from Configurables import ExtrapolationTest
extrapolationTest = ExtrapolationTest("ExtrapolationTest", OutputLevel=DEBUG)
extrapolationTest.extrapolationTool=extrapolationTool
extrapolationTest.ExtrapolatedTrackStates.Path="ExtrapolatedTrackstates"
extrapolationTest.genParticles.Path="allGenParticles"

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4GeantinoDeposits", actions="SimG4FullSimActions", magneticField=field)

geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4SaveTrajectory,  SimG4GeantinosFromEdmTool
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrajectorytool = SimG4SaveTrajectory("saveTrajectory")
savetrajectorytool.trajectory.Path = "trajectory"
savetrajectorytool.trajectoryPoints.Path = "trajectoryPoints"
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.positionedTrackHits.Path = "SimTrackerPositionedHits"
savetrackertool.trackHits.Path = "hits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
g4pconverter = SimG4GeantinosFromEdmTool()

geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveTrackerHits/saveTrackerHits", "SimG4SaveTrajectory/saveTrajectory" ],
                    eventProvider=g4pconverter)

## ouput
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]
out.filename = 'output_trkExtrapolationTest.root'


from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [ gen, hepmc_converter, extrapolationTest, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1000,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, trkgeoservice, geantservice],
                OutputLevel=DEBUG,
)


