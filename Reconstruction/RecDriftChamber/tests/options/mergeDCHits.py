import os
from Gaudi.Configuration import *


import GaudiKernel.SystemOfUnits as units
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input="root://eospublic.cern.ch//eos/experiment/fcc/ee/tutorial/fccee_idea_pgun.root")

from Configurables import PodioInput, ReadTestConsumer
podioinput = PodioInput("PodioReader", collections=["positionedHits_DCH"], OutputLevel=DEBUG)
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[os.environ.get("FCC_DETECTORS", "") + '/Detector/DetFCCeeIDEA/compact/FCCee_DectMaster.xml',])

from Configurables import CreateDCHHits
createhits = CreateDCHHits("CreateDCHHits",
                           readoutName = "DriftChamberCollection",
                           EdepCut = 100*1e-9,
                           DCACut = 0.8,
                           OutputLevel=INFO)

createhits.positionedHits.Path = "positionedHits_DCH"
createhits.mergedHits.Path = "merged_DCH"

from Configurables import PodioOutput
out = PodioOutput("out")
out.OutputLevel=DEBUG
out.outputCommands = ["keep *"]
out.filename="mergedDCHits.root"


ApplicationMgr( TopAlg = [
                          podioinput, 
                          createhits, 
                          out, 
                          ],
                EvtSel = 'NONE',
                EvtMax   = 20000,
                ExtSvc = [podioevent, geoservice ],
                OutputLevel = INFO
 )

