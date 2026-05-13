
from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants

from Configurables import EventDataSvc
from k4FWCore import ApplicationMgr, IOSvc

ApplicationMgr().EvtSel = 'NONE' 
ApplicationMgr().EvtMax = 2
ApplicationMgr().OutputLevel = INFO
ApplicationMgr().ExtSvc += ['RndmGenSvc']

## Data service
podioevent = EventDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]
