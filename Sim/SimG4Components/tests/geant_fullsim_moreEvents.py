

### \file
### \ingroup SimulationTests
### | **input (alg)**                 | other algorithms                   |                                     |                          |                                    | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | ----------------------------------- | ------------------------ | ---------------------------------- | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry taken from TestTracker.xml | FTFP_BERT physics list   | save Tracker and HCAL hits         | write the EDM output to ROOT file using PODIO   |




from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

podioevent = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:DetectorDescription/Detectors/compact/TestTracker.xml'],
                    OutputLevel = DEBUG)

from Configurables import G4SimSvc
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist="G4FtfpBert", actions="G4FullSimActions", )

from Configurables import G4SimAlg, G4SaveTrackerHits, G4SaveCalHits
savetrackertool = G4SaveTrackerHits("G4SaveTrackerHits")
savetrackertool.DataOutputs.trackClusters.Path = "clusters"
savetrackertool.DataOutputs.trackHits.Path = "hits"
savetrackertool.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"
geantsim = G4SimAlg("G4SimAlg",
                        outputs= ["G4SaveTrackerHits/G4SaveTrackerHits" ])
geantsim.DataInputs.genParticles.Path="allGenParticles"

from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg=[reader, hepmc_converter, geantsim, out],
                EvtSel='NONE',
                EvtMax=5,
                ## order! geo needed by geant
                ExtSvc=[podioevent, geoservice, geantservice],
                OutputLevel=DEBUG)
