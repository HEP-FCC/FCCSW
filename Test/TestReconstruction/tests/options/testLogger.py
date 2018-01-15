from Gaudi.Configuration import *
from Configurables import ApplicationMgr



from Configurables import TestACTSLogger
logtest = TestACTSLogger()

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[logtest],
               ExtSvc=[],
               OutputLevel=DEBUG)
