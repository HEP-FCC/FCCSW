## Simple Papas Run
## Runs papas using as a sequence of tools
## The reconstructed particles are written to a ROOT file

#
#  To run PapasTools
#  > ./run gaudirun.py Sim/SimPapas/options/simple_papastool.py
#

from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput
from GaudiKernel import SystemOfUnits as units

### Example of pythia configuration file to generate events
pythiafile="Test/TestGeneration/data/ee_ZH_Zmumu_Hbb.txt"

#### Data service
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import  HepMCFileReader
from Configurables import PythiaInterface, GenAlg
### PYTHIA algorithm
pythia8gentool = PythiaInterface("Pythia8Interface", Filename=pythiafile)
pythia8gen = GenAlg("Pythia8", SignalProvider=pythia8gentool)
pythia8gen.hepmc.Path = "hepmcevent"

from Configurables import HepMCToEDMConverter
### Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmcevent"
hepmc_converter.genparticles.Path="GenParticle"
hepmc_converter.genvertices.Path="Genvertex"

from  CMS_detector_cfg import detservice
from papas_cfg import papasalg

#output fcc particles to root
from Configurables import PodioOutput
out = PodioOutput("out",
                  OutputLevel=INFO)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr(
               ## all algorithms should be put here
               TopAlg=[pythia8gen, hepmc_converter, papasalg, out],
               EvtSel='NONE',
               ## number of events
               EvtMax=10,
               ## all services should be put here
               ExtSvc = [podioevent, detservice],
               OutputLevel = DEBUG
               )
