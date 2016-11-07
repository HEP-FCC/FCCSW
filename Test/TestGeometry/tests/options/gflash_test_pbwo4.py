from Gaudi.Configuration import *
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.7/testsamples/FCC_minbias_100TeV.dat")
## reads HepMC text file and write the HepMC::GenEvent to the data service
reader.DataOutputs.hepmc.Path = "hepmc"


from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
geoservice = GeoSvc("GeoSvc", detectors=['file:..//data/Barrel_testCaloSD_rphiz.xml'])

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
savecaltool.DataOutputs.positionedCaloHits.Path = "positionedCaloHits"
savecaltool.DataOutputs.caloHits.Path = "caloHits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.DataInputs.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveCalHits/saveCalHits"],
                    eventProvider=particle_converter)

from Configurables import PodioOutput
## PODIO algorithm
out = PodioOutput("out", filename = "out_fast_calo_100GeV_fullpi.root")
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [# gen

    reader
                          , hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice# , ppservice
                ],
                OutputLevel=DEBUG)
