from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc, AlbersWrite, AlbersOutput

albersevent   = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

dumper = HepMCDumper("Dumper")
dumper.DataInputs.hepmc.Path="hepmc"

alberswrite = AlbersWrite("AlbersWrite",OutputLevel=DEBUG)
alberswrite.DataOutputs.albersJets.Path = "albersJets"
out = AlbersOutput("out")


ApplicationMgr( TopAlg = [reader,dumper,alberswrite,out],
                EvtSel = 'NONE',
                EvtMax   = 3,
                ExtSvc = [albersevent],
#                EventLoop = eventloopmgr,
                OutputLevel=DEBUG
 )
