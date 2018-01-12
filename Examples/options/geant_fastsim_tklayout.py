from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import HepMCFileReader, GenAlg
readertool = HepMCFileReader("ReaderTool", Filename="/eos/project/f/fccsw-web/testsamples/FCC_minbias_100TeV.dat")
reader = GenAlg("Reader", SignalProvider=readertool)
reader.hepmc.Path = "hepmc"

# reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:Detector/DetFCChhBaseline1/compact/FCChh_TrackerAir.xml'],
                                         OutputLevel=INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc, SimG4FastSimPhysicsList, SimG4ParticleSmearRootFile, SimG4FastSimTrackerRegion
from GaudiKernel.SystemOfUnits import GeV, TeV
# create particle smearing tool, used for smearing in the tracker
smeartool = SimG4ParticleSmearRootFile("Smear", filename="/eos/project/f/fccsw-web/testsamples/tkLayout_example_resolutions.root")
## create region and a parametrisation model, pass smearing tool
regiontool = SimG4FastSimTrackerRegion("model", volumeNames=["TrackerEnvelopeBarrel"],
                                       minMomentum = 5*GeV, maxMomentum = 10*TeV, maxEta=6, smearing=smeartool)
# create overlay on top of FTFP_BERT physics list, attaching fast sim/parametrization process
physicslisttool = SimG4FastSimPhysicsList("Physics", fullphysics="SimG4FtfpBert")
# attach those tools to the G4 service
geantservice = SimG4Svc("SimG4Svc", physicslist=physicslisttool, regions=["SimG4FastSimTrackerRegion/model"])

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveSmearedParticles, SimG4PrimariesFromEdmTool
# first, create a tool that saves the smeared particles
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveSmearedParticles")
# and YY is the given name ("saveSmearedParticles")
saveparticlestool = SimG4SaveSmearedParticles("saveSmearedParticles")
saveparticlestool.particles.Path = "smearedParticles"
saveparticlestool.particlesMCparticles.Path = "particleMCparticleAssociation"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveSmearedParticles/saveSmearedParticles"],
                    eventProvider=particle_converter)


from Configurables import SimG4FastSimHistograms
hist = SimG4FastSimHistograms("fastHist")
hist.particlesMCparticles.Path = "particleMCparticleAssociation"
THistSvc().Output = ["rec DATAFILE='histTklayout.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out", filename = "out_fast_tklayout.root")
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, hist, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=INFO
 )
