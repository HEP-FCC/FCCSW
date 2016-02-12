from Gaudi.Configuration import *
# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

# reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import G4SimSvc
geantservice = G4SimSvc("G4SimSvc", detector="G4DD4hepDetector", physicslist="G4FtfpBert", actions="G4FullSimActions")

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:DetectorDescription/Detectors/compact/FCChh_DectMaster.xml',
                                         'file:DetectorDescription/Detectors/compact/FCChh_HCalBarrel.xml' ],
                    OutputLevel = DEBUG)

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import G4SimAlg, G4SaveCalHits
# first, create a tool that saves the calorimeter hits (of type "hcal")
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("G4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savehcaltool = G4SaveCalHits("saveHCalHits", caloType = "HCal")
savehcaltool.DataOutputs.caloClusters.Path = "caloClusters"
savehcaltool.DataOutputs.caloHits.Path = "caloHits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
geantsim = G4SimAlg("G4SimAlg",
                        outputs = ["G4SaveCalHits/saveHCalHits"] )
geantsim.DataInputs.genParticles.Path="allGenParticles"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
