
from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE' 
ApplicationMgr().EvtMax = 2
ApplicationMgr().OutputLevel = INFO
ApplicationMgr().ExtSvc += ['RndmGenSvc']

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")
ApplicationMgr().ExtSvc += [podioevent]
