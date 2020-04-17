from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import MomentumRangeParticleGun
guntool = MomentumRangeParticleGun()
from Configurables import GenAlg
gen = GenAlg()
gen.SignalProvider=guntool
gen.hepmc.Path = "hepmc"


from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter()
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="GenParticles"
hepmc_converter.genvertices.Path="GenVertices"

# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
geoservice = GeoSvc("GeoSvc", detectors=['file:Test/TestGeometry/data/Barrel_testCaloSD_rphiz.xml'])

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc, SimG4FastSimPhysicsList, SimG4GflashHomoCalo, SimG4FastSimCalorimeterRegion
## create parametrisation of the calorimeter
gflash = SimG4GflashHomoCalo("gflash", material = "G4_PbWO4")
regiontoolcalo = SimG4FastSimCalorimeterRegion("modelCalo", volumeNames=["BarrelECal"], parametrisation = gflash)
## create overlay on top of FTFP_BERT physics list, attaching fast sim/parametrization process
physicslisttool = SimG4FastSimPhysicsList("Physics", fullphysics="SimG4FtfpBert")
## attach those tools to the G4 service
geantservice = SimG4Svc("SimG4Svc", physicslist=physicslisttool, regions=["SimG4FastSimCalorimeterRegion/modelCalo"])

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveCalHits, SimG4PrimariesFromEdmTool
savecaltool = SimG4SaveCalHits("saveCalHits", readoutNames = ["ECalHits"])
savecaltool.positionedCaloHits.Path = "CaloHitsPositions"
savecaltool.caloHits.Path = "CaloHits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "GenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveCalHits/saveCalHits"],
                    eventProvider=particle_converter)


from Configurables import PodioOutput
out = PodioOutput("out")
out.filename = "out_fast_calo_100GeV_fullpi.root"
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice
                ],
                OutputLevel=DEBUG)
