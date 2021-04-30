
from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE' 
ApplicationMgr().EvtMax = 2
ApplicationMgr().OutputLevel = INFO
ApplicationMgr().ExtSvc = ['RndmGenSvc']

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]


from Configurables import MomentumRangeParticleGun
guntool = MomentumRangeParticleGun()
guntool.ThetaMin = 0 
guntool.ThetaMax = 2 * constants.pi 
guntool.PdgCodes = [11]

from Configurables import GenAlg
gen = GenAlg()
gen.SignalProvider=guntool
gen.hepmc.Path = "hepmc"
ApplicationMgr().TopAlg += [gen]

from Configurables import HepMCToEDMConverter
## reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.GenParticles.Path="allGenParticles"
ApplicationMgr().TopAlg += [hepmc_converter]


# DD4hep geometry service
from Configurables import GeoSvc
## parse the given xml file
path_to_detectors = os.environ.get("FCCDETECTORS", "")
geoservice = GeoSvc("GeoSvc")
geoservice.detectors = [
                          os.path.join(path_to_detectors, 'Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml'),
                          os.path.join(path_to_detectors, 'Detector/DetFCChhBaseline1/compact/FCChh_TrackerAir.xml'),
                          os.path.join(path_to_detectors, 'Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Gflash.xml'),
                       ]
geoservice.OutputLevel = INFO 
ApplicationMgr().ExtSvc += [geoservice]

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4ParticleSmearFormula
## create particle smearing tool, used for smearing in the tracker
smeartool = SimG4ParticleSmearFormula("smear")
smeartool.resolutionMomentum = "0.013"

from Configurables import SimG4FastSimTrackerRegion
## create region and a parametrisation model, pass smearing tool
regiontooltracker = SimG4FastSimTrackerRegion("modelTracker")
regiontooltracker.volumeNames = ["TrackerEnvelopeBarrel"]
regiontooltracker.smearing = smeartool

from Configurables import SimG4GflashSamplingCalo
## create parametrisation of the calorimeter
gflash = SimG4GflashSamplingCalo("gflash")
gflash.materialActive = "G4_lAr"
gflash.materialPassive = "G4_Pb"
gflash.thicknessActive = 4
gflash.thicknessPassive = 2

from Configurables import SimG4FastSimCalorimeterRegion
regiontoolcalo = SimG4FastSimCalorimeterRegion("modelCalo")
regiontoolcalo.volumeNames=["ECalBarrel"]
regiontoolcalo.parametrisation = gflash

from Configurables import SimG4FastSimPhysicsList
## create overlay on top of FTFP_BERT physics list, attaching fast sim/parametrization process
physicslisttool = SimG4FastSimPhysicsList("Physics")
physicslisttool.fullphysics = "SimG4FtfpBert"

from Configurables import SimG4Svc
## attach those tools to the G4 service
geantservice = SimG4Svc("SimG4Svc")
geantservice.physicslist = physicslisttool
geantservice.regions = [regiontooltracker, regiontoolcalo]
ApplicationMgr().ExtSvc += [geantservice]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# first, create a tool that saves the smeared particles
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveSmearedParticles")
# and YY is the given name ("saveSmearedParticles")
from Configurables import SimG4SaveSmearedParticles
saveparticlestool = SimG4SaveSmearedParticles("saveSmearedParticles")
saveparticlestool.RecParticles.Path = "smearedParticles"
saveparticlestool.MCRecoParticleAssoc.Path = "particleMCparticleAssociation"

from Configurables import SimG4SaveCalHits
savecaltool = SimG4SaveCalHits("saveCalHits")
savecaltool.readoutNames = ["ECalHitsPhiEta"]
#savecaltool.positionedCaloHits.Path = "positionedCaloHits"
#savecaltool.caloHits.Path = "caloHits"

from Configurables import SimG4PrimariesFromEdmTool
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.GenParticles.Path = "allGenParticles"

from Configurables import SimG4Alg
geantsim = SimG4Alg("SimG4Alg")
geantsim.outputs = [saveparticlestool, savecaltool]
geantsim.eventProvider = particle_converter
ApplicationMgr().TopAlg += [geantsim]


from Configurables import SimG4FastSimHistograms
hist = SimG4FastSimHistograms("fastHist")
hist.particlesMCparticles.Path = "particleMCparticleAssociation"
THistSvc().Output = ["rec DATAFILE='out_geant_fastsim_histFormula.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll = True
THistSvc().AutoSave = True
THistSvc().AutoFlush = True
THistSvc().OutputLevel = INFO
ApplicationMgr().TopAlg += [hist]


from Configurables import PodioOutput
out = PodioOutput("out")
out.filename = "out_geant_fastsim.root"
out.outputCommands = ["keep *"]
ApplicationMgr().TopAlg += [out]

