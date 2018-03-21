
### \file
### \ingroup SimulationExamples
### | **input (alg)**               | other algorithms                 |                                                         |                        |                                  | **output (alg)**                              |
### |-------------------------------|----------------------------------|---------------------------------------------------------|------------------------|----------------------------------|-----------------------------------------------|
### | generating single particle events from a given list of types, with momentum, phi and theta from a given range, saving to HepMC | convert `HepMC::GenEvent` to EDM | geometry parsed from XML (TestHCal.xml) by DD4hep using GeoSvc | FTFP_BERT physics list | saving HCal hits | write the EDM output to ROOT file using PODIO |

from Gaudi.Configuration import *

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")

### Example of pythia configuration file to generate events
#pythiafile="Generation/data/Pythia_standard.cmd"
pythiafile="/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Generation/data/Pythia_minbias_pp_100TeV.cmd"
# Example of pythia configuration file to read LH event file
#pythiafile="options/Pythia_LHEinput.cmd"

from Configurables import FCCDataSvc
#### Data service
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import ConstPileUp

#pileuptool = ConstPileUp(numPileUpEvents=<NPILEUPEVENTS>, Filename="/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Generation/data/Pythia_minbias_pp_100TeV.cmd")

from Configurables import PythiaInterface, GenAlg
### PYTHIA algorithm
pythia8gentool = PythiaInterface("Pythia8Interface", Filename=pythiafile)
pythia8gen = GenAlg("Pythia8", SignalProvider=pythia8gentool)
pythia8gen.hepmc.Path = "hepmcevent"

gen = GenAlg("ParticleGun", SignalProvider=pythia8gentool)
gen.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
### Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import GenParticleFilter
### Filters generated particles
#genfilter = GenParticleFilter("StableParticles")
#genfilter.genparticles_in.Path = "all_genparticles"
#genfilter.genparticles_out.Path = "StableParticles"


from Configurables import GeoSvc
## DD4hep geometry service
# Parses the given xml file
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Detector/DetFCChhECalDigital/compact/FCChh_DECalBarrel_<CONFIG>.xml'
                                        ],
                    OutputLevel = INFO)

from Configurables import SimG4Svc
## Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions")


# Magnetic field
from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",FieldOn=True,IntegratorStepper="ClassicalRK4",FieldComponentZ="-0.004")


from Configurables import SimG4Alg, SimG4SaveCalHits, SimG4PrimariesFromEdmTool
## Geant4 algorithm
saveecaltool = SimG4SaveCalHits("saveECalHits",readoutNames = ["BarDECal_Readout"])
saveecaltool.positionedCaloHits.Path = "positionedCaloHits"
saveecaltool.caloHits.Path = "ECalHits"

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"

geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveCalHits/saveECalHits"],
                    eventProvider=particle_converter)

from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[gen, hepmc_converter, geantsim, out],
                EvtSel='NONE',
                EvtMax=<NEVENTS>,
                ## order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc=[podioevent, geoservice, geantservice],
                OutputLevel=INFO
 )
