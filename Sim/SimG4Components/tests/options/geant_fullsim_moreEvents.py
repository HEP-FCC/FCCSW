

### \file
### \ingroup SimulationTests
### | **input (alg)**                 | other algorithms                   |                                     |                          |                                    | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | ----------------------------------- | ------------------------ | ---------------------------------- | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry taken from TestTracker.xml | FTFP_BERT physics list   | save Tracker and HCAL hits         | write the EDM output to ROOT file using PODIO   |




from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCDumper, FCCDataSvc

podioevent = FCCDataSvc("EventDataSvc")

from Configurables import HepMCFileReader, GenAlg
readertool = HepMCFileReader("ReaderTool", Filename="/eos/project/f/fccsw-web/testsamples/FCC_minbias_100TeV.dat")
reader = GenAlg("Reader", SignalProvider=readertool)
reader.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:../../../Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:../../../Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'],
                    OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions", )

from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
savetrackertool = SimG4SaveTrackerHits("SimG4SaveTrackerHits", readoutNames=["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"

particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/SimG4SaveTrackerHits"],
                    eventProvider=particle_converter)

from Configurables import PodioOutput
out = PodioOutput("out",
                  filename = "out_full_moreEvents.root",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg=[reader, hepmc_converter, geantsim, out],
                EvtSel='NONE',
                EvtMax=10,
                ## order! geo needed by geant
                ExtSvc=[podioevent, geoservice, geantservice],
                OutputLevel=DEBUG)
