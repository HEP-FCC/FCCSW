from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc, PodioWrite, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

dumper = HepMCDumper("Dumper")
dumper.DataInputs.hepmc.Path="hepmc"

podiowrite = PodioWrite("PodioWrite",OutputLevel=DEBUG)
podiowrite.DataOutputs.podioJets.Path = "podioJets"
out = PodioOutput("out", OutputLevel=DEBUG)


ApplicationMgr( TopAlg = [reader,dumper,podiowrite,out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [podioevent],
#                EventLoop = eventloopmgr,
                OutputLevel=INFO
 )
