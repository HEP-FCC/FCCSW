from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCReader, HepMCDumper, PoissonPileUp, HepMCFileReader
genpileup = PoissonPileUp(name="ConstPileUp", filename="/eos/project/f/fccsw-web/testsamples/example_MyPythia.dat", numPileUpEvents=1)
reader = HepMCReader("Reader", Filename="/eos/project/f/fccsw-web/testsamples/example_MyPythia.dat", PileUpTool=genpileup)
# have a look at the source code of HepMCReader, in Generation/src/HepMCReader
# In the following line,
#   reader.YYY.Path = "ZZZ"
# YYY matches the string passed to declareOutput in the constructor of the algorithm
# XXX declares a name for the product (the HepMC::GenEvent)
reader.hepmc.Path = "hepmc"


dumper = HepMCDumper()
dumper.hepmc.Path="hepmc"

# reads an HepMC::GenEvent from the data service and writes
# a collection of EDM Particles
from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
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

