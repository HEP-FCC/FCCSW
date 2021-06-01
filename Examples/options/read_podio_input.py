from Gaudi.Configuration import *

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE'
ApplicationMgr().EvtMax = 3

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
podioevent.input = "tracker_with_field.root"
ApplicationMgr().ExtSvc += [podioevent]

from Configurables import PodioInput, ReadTestConsumer
podioinput = PodioInput("PodioReader")
podioinput.collections = ["allGenVertices", "allGenParticles", "hits"]
podioinput.OutputLevel = DEBUG
ApplicationMgr().TopAlg += [podioinput]

from Configurables import ReadTestConsumer
checker = ReadTestConsumer()
ApplicationMgr().TopAlg += [checker]

out = PodioOutput("out")
out.filename = "out2.root"
out.OutputLevel = DEBUG
out.outputCommands = ["keep *"]
ApplicationMgr().TopAlg += [out]

