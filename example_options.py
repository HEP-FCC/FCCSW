from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, AlbersDataSvc, AlbersWrite

albersevent   = AlbersDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.Outputs.hepmc.Path = "hepmc"

dumper = HepMCDumper("Dumper")
dumper.Inputs.hepmc.Path="hepmc"

alberswrite = AlbersWrite("AlbersWrite")
alberswrite.Outputs.albersJets.Path = "albersJets"


ApplicationMgr( TopAlg = [reader,dumper,alberswrite],
                EvtSel = 'NONE',
                EvtMax   = 2,
                ExtSvc = [albersevent],
#                EventLoop = eventloopmgr,
OutputLevel=DEBUG
 )
