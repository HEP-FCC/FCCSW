

### \file
### \ingroup SimulationTests
### | **input (alg)**                 | other algorithms                   |                                     |                          |                                    | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | ----------------------------------- | ------------------------ | ---------------------------------- | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry taken from TestTracker.xml | FTFP_BERT physics list   | save Tracker and HCAL hits         | write the EDM output to ROOT file using PODIO   |




from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

podioevent = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.7/testsamples/FCC_minbias_100TeV.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:../../../Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:../../../Detector/DetFCChhTrackerSimple/compact/Tracker.xml'],
                    OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions", )

from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
savetrackertool = SimG4SaveTrackerHits("SimG4SaveTrackerHits", readoutNames=["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.DataOutputs.trackClusters.Path = "clusters"
savetrackertool.DataOutputs.trackHits.Path = "hits"
savetrackertool.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"

particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.DataInputs.genParticles.Path = "allGenParticles"
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
