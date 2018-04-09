from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input="output_hcalSim_e50GeV_eta036_1events.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["HCalHits"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool
calibHcells = CalibrateCaloHitsTool("CalibrateHCal", invSamplingFraction="41.7 ")

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              calibTool=calibHcells,
                              doCellCalibration = True,
                              addCellNoise = False, filterCellNoise = False,
                              OutputLevel = DEBUG)
createcells.hits.Path="HCalHits"
createcells.cells.Path="HCalCells"

out = PodioOutput("out", filename="output_HCalCells_digitisation_noNoise.root",
                   OutputLevel = DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createcells.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              createcells,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [podioevent, geoservice],
 )

