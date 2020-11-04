"""
Pythia8, integrated in the FCCSW framework.

Generates according to a pythia .cmd file and saves them in fcc edm format.

"""

import os
from GaudiKernel import SystemOfUnits as units
from Gaudi.Configuration import *

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE' 
ApplicationMgr().EvtMax = 2
ApplicationMgr().OutputLevel = INFO

#### Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]

from Configurables import GaussSmearVertex
smeartool = GaussSmearVertex()
smeartool.xVertexSigma =   0.5*units.mm
smeartool.yVertexSigma =   0.5*units.mm
smeartool.zVertexSigma =  40.0*units.mm
smeartool.tVertexSigma = 180.0*units.picosecond

from Configurables import PythiaInterface
pythia8gentool = PythiaInterface()
### Example of pythia configuration file to generate events
pythiafilename = "Generation/data/Pythia_standard.cmd"
path_to_pythiafile = os.environ.get("FCCSW_SHARE_DIR", "")
pythiafile = os.path.join(path_to_pythiafile, pythiafilename)
# Example of pythia configuration file to read LH event file
#pythiafile="options/Pythia_LHEinput.cmd"
pythia8gentool.Filename = pythiafile
pythia8gentool.doEvtGenDecays = False
pythia8gentool.printPythiaStatistics = True

from Configurables import GenAlg
pythia8gen = GenAlg("Pythia8")
pythia8gen.SignalProvider = pythia8gentool
pythia8gen.VertexSmearingTool = smeartool
pythia8gen.hepmc.Path = "hepmc"
ApplicationMgr().TopAlg += [pythia8gen]

### Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter()
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.hepmcStatusList = [] # convert particles with all statuses
hepmc_converter.genparticles.Path="GenParticles"
hepmc_converter.genvertices.Path="GenVertices"
ApplicationMgr().TopAlg += [hepmc_converter]

### Filters generated particles
# accept is a list of particle statuses that should be accepted
from Configurables import GenParticleFilter
genfilter = GenParticleFilter("StableParticles")
genfilter.accept = [1]
genfilter.allGenParticles.Path = "GenParticles"
genfilter.filteredGenParticles.Path = "GenParticlesStable"
ApplicationMgr().TopAlg += [genfilter]

from Configurables import PodioOutput
out = PodioOutput("out")
out.outputCommands = ["keep *"]
ApplicationMgr().TopAlg += [out]


