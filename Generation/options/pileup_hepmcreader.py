from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc

albersevent   = FCCDataSvc("EventDataSvc")
# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import HepMCReader, HepMCDumper, PoissonPileUp, HepMCFileReader
genpileup = PoissonPileUp(name="Pileup", 
    Filename="/afs/cern.ch/user/v/vavolkl/public/example_MyPythia.dat", 
    numPileUpEvents=2)
reader = HepMCReader("Reader", 
    Filename="/afs/cern.ch/user/v/vavolkl/public/example_MyPythia.dat", 
    PileUpTool=genpileup)
reader.DataOutputs.hepmc.Path = "hepmc"

# printout of the hepmc stuff
dumper = HepMCDumper()
dumper.DataInputs.hepmc.Path="hepmc"

ApplicationMgr(
    TopAlg = [reader, dumper],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [albersevent],
 )

