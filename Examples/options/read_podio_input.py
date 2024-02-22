from Gaudi.Configuration import *

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE'
ApplicationMgr().EvtMax = 2

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
podioevent.input = "http://fccsw.web.cern.ch/fccsw/testsamples/out_particle_gun.root"
ApplicationMgr().ExtSvc += [podioevent]

from Configurables import PodioInput
podioinput = PodioInput("PodioReader")
podioinput.collections = ["GenParticles"]
podioinput.OutputLevel = DEBUG
ApplicationMgr().TopAlg += [podioinput]


from Configurables import PodioOutput
out = PodioOutput("out")
out.filename = "out_read_podio_input.root"
out.OutputLevel = DEBUG
out.outputCommands = ["keep *"]
ApplicationMgr().TopAlg += [out]

