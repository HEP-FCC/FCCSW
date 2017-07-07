from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml'],
                    OutputLevel = INFO)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")
geantservice.g4PostInitCommands  += ["/run/setCut 0.1 mm"]
from Configurables import SimG4Alg, SimG4SaveCalHits, SimG4SingleParticleGeneratorTool
saveecaltool = SimG4SaveCalHits("saveECalHits",readoutNames = ["ECalHitsEta"])
saveecaltool.positionedCaloHits.Path = "ECalPositionedHits"
saveecaltool.caloHits.Path = "ECalHits"
from Configurables import SimG4SingleParticleGeneratorTool
pgun=SimG4SingleParticleGeneratorTool("SimG4SingleParticleGeneratorTool",saveEdm=True,
                                      particleName = "e-", energyMin = 50000, energyMax = 50000, etaMin = -1.5, etaMax = 1.5)
geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveCalHits/saveECalHits"],
                    eventProvider = pgun)

#Configure tools for calo reconstruction
from Configurables import CalibrateInLayersTool
calibcells = CalibrateInLayersTool("Calibrate",
                                   # sampling fraction obtained using SamplingFractionInLayers from DetStudies package
                                   samplingFraction = [0.168] * 4 + [0.176] * 18 + [0.184] * 18 + [0.191] * 18 + [0.198] * 18 + [0.204] * 18 + [0.210] * 18 + [0.215] * 18,
                                   readoutName = "ECalHitsEta",
                                   layerFieldName = "layer")

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              doCellCalibration=True,
                              calibTool=calibcells,
                              addCellNoise=False, filterCellNoise=False,
                              OutputLevel=DEBUG)
createcells.hits.Path="ECalHits"
createcells.cells.Path="caloCells"

out = PodioOutput("out", filename="output_ecalInclinedDigi_test.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True
createcells.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [geantsim,
              createcells,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [podioevent, geoservice],
 )

