from Gaudi.Configuration import *
from Configurables import ApplicationMgr, TestSavingAlgorithm, TestReadingAlgorithm, FCCDataSvc, ToolWithOutput

albersevent = FCCDataSvc("EventDataSvc")

tool = ToolWithOutput("ToolWithOutput")
tool.DataOutputs.particles.Path = "hepmc"
save = TestSavingAlgorithm("Save", ToolWithOutput=tool)

read = TestReadingAlgorithm("Read")
read.DataInputs.particles.Path = "hepmc"

from Configurables import AlbersWrite, AlbersOutput
out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [save, read, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [],
                OutputLevel=DEBUG
 )
