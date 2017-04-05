
### \file
### \ingroup SimulationExamples
### | **input (alg)**               | other algorithms                 |                                                         |                        |                                  | **output (alg)**                              |
### |-------------------------------|----------------------------------|---------------------------------------------------------|------------------------|----------------------------------|-----------------------------------------------|
### | read events from a HepMC file | convert `HepMC::GenEvent` to EDM | geometry taken from GDML file (no sensitive detectors!) | FTFP_BERT physics list | empty action initialisation list | write the EDM output to ROOT file using PODIO |


from Gaudi.Configuration import *


# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
# from Configurables import HepMCReader
from Configurables import HepEVTReader
# reader = HepEVTReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.7/testsamples/FCC_minbias_100TeV.dat")
#reader = HepEVTReader("Reader", Filename="/afs/cern.ch/user/v/voutsina/Work/PairsTop/HepEvtFiles/pairs_top13.hepevt")
reader = HepEVTReader("Reader", Filename="/afs/cern.ch/user/v/voutsina/Work/PairsTop/HepEvtFiles/pairs_top"+FILENUM+".hepevt")
reader.DataOutputs.genparticles.Path="allGenParticles"
reader.DataOutputs.genvertices.Path="allGenVertices"

    
# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc

"""
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:Detector/DetFCChhBaseline1/compact/FCChh_BeamTube.xml',
                                         'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'])
"""
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/FCCeeOriginal/compact/FCCee_o3_v03.xml']) 

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="HelixSimpleRunge",
                                       FieldComponentZ=0.002, MinimumEpsilon=1)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions",
                        magneticField=field)
geantservice.G4commands += ["/run/setCut 0.1 mm"]
geantservice.G4commands +=["/process/eLoss/minKinEnergy 1 MeV"]


# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
# savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["VertexBarrelCollection", "VertexEndcapCollection"])
                                                                          #"InnerTrackerBarrelCollection", "InnerTrackerEndcapCollection", 
                                                                          #"OuterTrackerBarrelCollection", "OuterTrackerEndcapCollection"])

savetrackertool.DataOutputs.positionedTrackHits.Path = "positionedHits"
savetrackertool.DataOutputs.trackHits.Path = "hits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.DataInputs.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits"],
                    eventProvider=particle_converter)

# PODIO algorithm

from Configurables import PodioOutput
out = PodioOutput("out",
                  filename="simuOutput_boost/output"+FILENUM+".root",
                  # filename="test_2T.root",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]


# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, geantsim, out], #, hepmc_converter],
                EvtSel = "NONE",
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=INFO
                )

"""# reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepEVTConverter
hepmc_converter = HepEVTConverter("Converter")
hepmc_converter.DataInputs.hepevt.Path="hepevt"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
#hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

"""




"""
# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
#geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml', 'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'])
#geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCCeeBaseline1/compact/FCCee_o3_v03.xml'])
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/FCCeeOriginal/compact/FCCee_o3_v03.xml'])


# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["VertexBarrelCollection", "VertexEndcapCollection"])
savetrackertool.DataOutputs.positionedTrackHits.Path = "positionedHits"
savetrackertool.DataOutputs.trackHits.Path = "hits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.DataInputs.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits"],
                    eventProvider=particle_converter)

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 10,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=INFO
 )

"""

