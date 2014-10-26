from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc, AlbersWrite, AlbersOutput

albersevent   = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.Outputs.hepmc.Path = "hepmc"

dumper = HepMCDumper("Dumper")
dumper.Inputs.hepmc.Path="hepmc"

alberswrite = AlbersWrite("AlbersWrite")
alberswrite.Outputs.albersJets.Path = "albersJets"
out = AlbersOutput("out")


ApplicationMgr( TopAlg = [reader,dumper,alberswrite,out],
                EvtSel = 'NONE',
                EvtMax   = 2,
                ExtSvc = [albersevent],
#                EventLoop = eventloopmgr,
OutputLevel=DEBUG
 )
