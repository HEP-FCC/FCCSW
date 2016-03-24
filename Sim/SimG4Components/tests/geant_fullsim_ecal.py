
### \file
### \ingroup SimulationTests
### | **input (alg)**                 | other algorithms                   |                                                           |                          |                                    | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | --------------------------------------------------------- | ------------------------ | ---------------------------------- | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry taken from XML - main + ECAL                     | FTFP_BERT physics list   | save ECAL + HCAL hits              | write the EDM output to ROOT file using PODIO   |


from Gaudi.Configuration import *
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import HepMCReader
## reads HepMC text file and write the HepMC::GenEvent to the data service
reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
## reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import G4SimSvc
## Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
geantservice = G4SimSvc("G4SimSvc", detector="G4DD4hepDetector", physicslist="G4FtfpBert", actions="G4FullSimActions")

from Configurables import GeoSvc
## DD4hep geometry service
# Parses the given xml file
geoservice = GeoSvc("GeoSvc", detectors=['file:DetectorDescription/Detectors/compact/FCChh_DectMaster.xml',
                                         'file:DetectorDescription/Detectors/compact/FCChh_ECalBarrel_Mockup.xml' ],
                    OutputLevel = DEBUG)

from Configurables import G4SimAlg, G4SaveCalHits
## Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# first, create a tool that saves the calorimeter hits (of type "ecal")
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("G4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
saveecaltool = G4SaveCalHits("saveECalHits", caloType = "ECal")
saveecaltool.DataOutputs.caloClusters.Path = "caloClusters"
saveecaltool.DataOutputs.caloHits.Path = "caloHits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
geantsim = G4SimAlg("G4SimAlg",
                        outputs = ["G4SaveCalHits/saveECalHits"] )
geantsim.DataInputs.genParticles.Path="allGenParticles"

from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[reader, hepmc_converter, geantsim, out],
                EvtSel='NONE',
                EvtMax=1,
                ## order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc=[podioevent, geoservice, geantservice],
                OutputLevel=DEBUG)
