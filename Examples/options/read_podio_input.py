from Gaudi.Configuration import *

from Configurables import EventDataSvc
from k4FWCore import ApplicationMgr, IOSvc

ApplicationMgr().EvtSel = 'NONE'
ApplicationMgr().EvtMax = 2

podioevent = EventDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]

iosvc = IOSvc()
iosvc.Input = "http://fccsw.web.cern.ch/fccsw/testsamples/out_particle_gun.root"
iosvc.CollectionNames = ["GenParticles"]
iosvc.Output = "out_read_podio_input.root"
iosvc.outputCommands = ["keep *"]

