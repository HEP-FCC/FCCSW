## Simple Papas Run for CLIC
## Runs papas using as a sequence of tools
## The reconstructed particles are written to a ROOT file

#
#  To run PapasTools
#  > ./run gaudirun.py Sim/SimPapas/options/simple_CLIC.py
#

from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput
from GaudiKernel import SystemOfUnits as units
import argparse
recoparser = argparse.ArgumentParser()
recoparser.add_argument('-r','--rootfile', type=str, default=None, help='rootfile')
recoargs, _ = recoparser.parse_known_args()
rootfile = recoargs.rootfile
print "=================================="
print "rootfile = ", rootfile
print "=================================="



## read in generated particles from ROOT via podio
podioevent   = FCCDataSvc("EventDataSvc", input=rootfile) #"Sim/SimPapas/data/ee_Z_ddbar.root")

from Configurables import PodioInput, ReadTestConsumer
podioinput = PodioInput("PodioReader", collections=["GenVertex", "GenParticle"], OutputLevel=DEBUG)

from CLIC_detector_cfg import detservice
from papas_cfg import papasalg_pdebug

#output fcc particles to root
from Configurables import PodioOutput
out = PodioOutput("out",
                  OutputLevel=INFO)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr(
    ## all algorithms should be put here
    TopAlg=[podioinput, papasalg_pdebug, out],
    EvtSel='NONE',
    ## number of events
    EvtMax=10,
    ## all services should be put here
    ExtSvc = [podioevent, detservice],
    OutputLevel = DEBUG
 )
