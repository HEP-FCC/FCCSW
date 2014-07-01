from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.Outputs.hepmc.Path = "hepmc"

dumper = HepMCDumper("Dumper")
dumper.Inputs.hepmc.Path="hepmc"

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[reader,dumper])
