from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")

## create DD4hep geometry
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'])
## create tracking geometry
from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")


from Configurables import GenAlg, MomentumRangeParticleGun
## Particle Gun using MomentumRangeParticleGun tool and FlatSmearVertex
# MomentumRangeParticleGun generates particles of given type(s) within given momentum, phi and theta range
# FlatSmearVertex smears the vertex with uniform distribution
pgun_tool = MomentumRangeParticleGun(PdgCodes=[13], ThetaMin=0., ThetaMax=3.14)
gen = GenAlg("ParticleGun", SignalProvider=pgun_tool, VertexSmearingTool="FlatSmearVertex")
gen.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
## Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

## Set up the magnetic field service
from Configurables import InterpolatedBFieldSvc
bFieldSvc = InterpolatedBFieldSvc("BFieldSvc", fieldMap = '/eos/experiment/fcc/hh/simulation/MagneticField/FCChhBField_rz.root', treeName= 'bField', lengthScalor=1., bFieldScalor= 1., cylinderCoordinates = TRUE, firstOctant=TRUE, OutputLevel = INFO)

## configure the extrapolation tool
from Configurables import ExtrapolationTool
extrapolationTool = ExtrapolationTool("ExtrapolationTool")
extrapolationTool.trackingGeometrySvc   = trkgeoservice
extrapolationTool.magneticFieldSvc      = bFieldSvc
extrapolationTool.collectSensitive      = TRUE
extrapolationTool.collectPassive        = TRUE
extrapolationTool.collectBoundary       = TRUE
extrapolationTool.collectMaterial       = TRUE
extrapolationTool.sensitiveCurvilinear  = FALSE
extrapolationTool.searchMode            = 1
extrapolationTool.pathLimit             = -1.

## configure the extrapolation test
from Configurables import ExtrapolationTest
extrapolationTest = ExtrapolationTest("ExtrapolationTest", OutputLevel=INFO)
extrapolationTest.extrapolationTool=extrapolationTool
extrapolationTest.positionedTrackHits.Path="positionedHits"
extrapolationTest.genParticles.Path="allGenParticles"

## ouput
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]
out.filename = 'output_trkExtrapolationTest.root'


from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [ gen, hepmc_converter, extrapolationTest, out],
                EvtSel = 'NONE',
                EvtMax   = 1000,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, trkgeoservice, bFieldSvc],
                OutputLevel=INFO,
 )
