import os

from Gaudi.Configuration import *

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE' 
ApplicationMgr().EvtMax = 2
ApplicationMgr().OutputLevel = INFO
ApplicationMgr().ExtSvc += ['RndmGenSvc']

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]

from Configurables import MomentumRangeParticleGun
from GaudiKernel import PhysicalConstants as constants
guntool = MomentumRangeParticleGun()
guntool.ThetaMin = 0 
guntool.ThetaMax = 2 * constants.pi 
guntool.PdgCodes = [11]
from Configurables import FlatSmearVertex
vertexsmeartool = FlatSmearVertex()
vertexsmeartool.xVertexMin = -25.
vertexsmeartool.xVertexMax = 25.
vertexsmeartool.yVertexMin = -25.
vertexsmeartool.yVertexMax = 25.
vertexsmeartool.zVertexMin = -25.
vertexsmeartool.zVertexMax = 25.
from Configurables import GenAlg
gen = GenAlg()
gen.SignalProvider = guntool
gen.VertexSmearingTool = vertexsmeartool
gen.hepmc.Path = "hepmc"
ApplicationMgr().TopAlg += [gen]


# reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.GenParticles.Path="GenParticles"
ApplicationMgr().TopAlg += [hepmc_converter]


# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
path_to_detectors = os.environ.get("FCCDETECTORS", "")
geoservice = GeoSvc("GeoSvc")
geoservice.detectors = [
                          os.path.join(path_to_detectors, 'Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml'),
                          os.path.join(path_to_detectors, 'Detector/DetFCChhBaseline1/compact/FCChh_TrackerAir.xml'),
                       ]
geoservice.OutputLevel = INFO 
ApplicationMgr().ExtSvc += [geoservice]


# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from GaudiKernel.SystemOfUnits import GeV, TeV
# create particle smearing tool, used for smearing in the tracker
from Configurables import SimG4ParticleSmearRootFile
smeartool = SimG4ParticleSmearRootFile("Smear")
smeartool.filename = "http://fccsw.web.cern.ch/fccsw/testsamples/tkLayout_example_resolutions.root"
## create region and a parametrisation model, pass smearing tool
from Configurables import SimG4FastSimTrackerRegion
regiontool = SimG4FastSimTrackerRegion("model")
regiontool.volumeNames = ["TrackerEnvelopeBarrel"]
regiontool.minMomentum = 5*GeV
regiontool.maxMomentum = 10*TeV
regiontool.maxEta = 6
regiontool.smearing = smeartool
# create overlay on top of FTFP_BERT physics list, attaching fast sim/parametrization process
from Configurables import SimG4FastSimPhysicsList
physicslisttool = SimG4FastSimPhysicsList("Physics")
physicslisttool.fullphysics="SimG4FtfpBert"
# attach those tools to the G4 service
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")
geantservice.physicslist = physicslisttool
geantservice.regions = [regiontool]
ApplicationMgr().ExtSvc += [geantservice]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveSmearedParticles, SimG4PrimariesFromEdmTool
# first, create a tool that saves the smeared particles
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveSmearedParticles")
# and YY is the given name ("saveSmearedParticles")
from Configurables import SimG4SaveSmearedParticles
saveparticlestool = SimG4SaveSmearedParticles("saveSmearedParticles")
saveparticlestool.RecParticles.Path = "smearedParticles"
saveparticlestool.MCRecoParticleAssoc.Path = "particleMCparticleAssociation"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
from Configurables import SimG4PrimariesFromEdmTool
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.GenParticles.Path = "GenParticles"
from Configurables import SimG4Alg
geantsim = SimG4Alg("SimG4Alg")
geantsim.outputs = [saveparticlestool]
geantsim.eventProvider = particle_converter
ApplicationMgr().TopAlg += [geantsim]


from Configurables import SimG4FastSimHistograms
hist = SimG4FastSimHistograms("fastHist")
hist.particlesMCparticles.Path = "particleMCparticleAssociation"
THistSvc().Output = ["rec DATAFILE='out_geant_fastsim_tklayout_histTklayout.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll = True
THistSvc().AutoSave = True
THistSvc().AutoFlush = True
ApplicationMgr().TopAlg += hist

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput() 
out.filename = "out_geant_fastsim_tklayout.root"
out.outputCommands = ["keep *"]
ApplicationMgr().TopAlg += [out]

