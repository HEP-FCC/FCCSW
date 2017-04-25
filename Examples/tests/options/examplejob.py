from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import GenAlg, HepMCDumper, PoissonPileUp, HepMCFileReader
genpileup = PoissonPileUp(name="ConstPileUp", numPileUpEvents=1)
readertool = HepMCFileReader("ReaderTool", Filename="/eos/project/f/fccsw-web/testsamples/example_MyPythia.dat")
readertool_pileup = HepMCFileReader("ReaderToolPileup", Filename="/eos/project/f/fccsw-web/testsamples/example_MyPythia.dat")
reader = GenAlg("Reader", SignalProvider=readertool, PileUpProvider=readertool_pileup, PileUpTool=genpileup)
# have a look at the source code of GenAlg, in Generation/src/components/GenAlg.cpp
# In the following line,
#   reader.YYY.Path = "ZZZ"
# YYY matches the string passed to declareOutput in the constructor of the algorithm
# XXX declares a name for the product (the HepMC::GenEvent)
reader.hepmc.Path = "hepmc"


dumper = HepMCDumper()
dumper.hepmc.Path="hepmc"

# reads an HepMC::GenEvent from the data service and writes
# a collection of EDM Particles
from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
# the input product name matches the output product name of the previous module
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="all_genparticles"
hepmc_converter.genvertices.Path="all_genvertices"


out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [reader, dumper,hepmc_converter,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 10,
    ExtSvc = [podioevent],
 )

