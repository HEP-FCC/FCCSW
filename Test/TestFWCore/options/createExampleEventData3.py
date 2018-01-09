from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import CreateExampleEventData
producer = CreateExampleEventData()
producer.magicNumberOffset = 200

from Configurables import PodioOutput
out = PodioOutput("out")
out.filename = "dummyEventData3.root"
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[producer, out],
                EvtSel="NONE",
                EvtMax=10,
                ExtSvc=[podioevent],
                OutputLevel=INFO,
                )


