## Simple Papas Run for CMS
## Runs papas using as a sequence of tools
## The reconstructed particles are written to a ROOT file

#
#  To run PapasTools
#  > ./run gaudirun.py Sim/SimPapas/options/simple_CMS.py
#

from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput
from GaudiKernel import SystemOfUnits as units

## read in generated particles from ROOT via podio
#podioevent   = FCCDataSvc("EventDataSvc", input="./ee_ZH_Zmumu_Hbb.root")
podioevent   = FCCDataSvc("EventDataSvc", input="./ee_Z_bbbar.root")

from Configurables import PodioInput, ReadTestConsumer
podioinput = PodioInput("PodioReader", collections=["GenVertex", "GenParticle"], OutputLevel=DEBUG)

from  CMS_detector_cfg import detservice
from papas_cfg import papasalg

#output fcc particles to root
from Configurables import PodioOutput
out = PodioOutput("out",
                  OutputLevel=INFO)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr(
    ## all algorithms should be put here
    TopAlg=[podioinput, papasalg, out],
    EvtSel='NONE',
    ## number of events
    EvtMax=100,
    ## all services should be put here
    ExtSvc = [podioevent, detservice],
    OutputLevel = DEBUG
 )
