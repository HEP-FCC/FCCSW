from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                           'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml' ],
                    OutputLevel = INFO)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")
geantservice.g4PostInitCommands  += ["/run/setCut 0.1 mm"]
from Configurables import SimG4Alg, SimG4SaveCalHits, SimG4SingleParticleGeneratorTool
saveecaltool = SimG4SaveCalHits("saveECalBarrelHits",readoutNames = ["ECalBarrelEta"])
saveecaltool.positionedCaloHits.Path = "ECalBarrelPositionedHits"
saveecaltool.caloHits.Path = "ECalBarrelHits"
savecalendcaptool = SimG4SaveCalHits("saveECalEndcapHits", readoutNames = ["EMECPhiEta"])
savecalendcaptool.positionedCaloHits.Path = "ECalEndcapPositionedHits"
savecalendcaptool.caloHits.Path = "ECalEndcapHits"
from Configurables import SimG4SingleParticleGeneratorTool
pgun=SimG4SingleParticleGeneratorTool("SimG4SingleParticleGeneratorTool",saveEdm=True,
                                      particleName = "e-", energyMin = 50000, energyMax = 50000, etaMin = 2., etaMax = 2.)
geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveCalHits/saveECalBarrelHits", "SimG4SaveCalHits/saveECalEndcapHits"],
                    eventProvider = pgun)

#Configure tools for calo reconstruction
from Configurables import CalibrateInLayersTool
calibcellsBarrel = CalibrateInLayersTool("CalibrateBarrel",
                                   # sampling fraction obtained using SamplingFractionInLayers from DetStudies package
                                   samplingFraction = [0.12125] * 4 + [0.14283] * 18 + [0.16354] * 18 + [0.17662] * 18 + [0.18867] * 18 + [0.19890] * 18 + [0.20637] * 18 + [0.20802] * 18,
                                   readoutName = "ECalBarrelEta",
                                   layerFieldName = "layer")
calibcellsEndcap = CalibrateInLayersTool("CalibrateEndcap",
                                         # sampling fraction obtained using SamplingFractionInLayers from DetStudies package
                                    samplingFraction = [0.15] * 118,
                                    readoutName = "EMECPhiEta",
                                    layerFieldName = "layer")

from Configurables import CreateCaloCells
createcellsBarrel = CreateCaloCells("CreateCaloCellsBarrel",
                                    doCellCalibration=True,
                                    calibTool=calibcellsBarrel,
                                    addCellNoise=False, filterCellNoise=False,
                                    OutputLevel=DEBUG)
createcellsBarrel.hits.Path="ECalBarrelHits"
createcellsBarrel.cells.Path="ECalBarrelCells"
createcellsEndcap = CreateCaloCells("CreateCaloCellsEndcap",
                                    doCellCalibration=True,
                                    calibTool=calibcellsEndcap,
                                    addCellNoise=False, filterCellNoise=False,
                                    OutputLevel=DEBUG)
createcellsEndcap.hits.Path="ECalEndcapHits"
createcellsEndcap.cells.Path="ECalEndcapCells"

out = PodioOutput("out", filename="output_ecalInclinedWithEndcapDigi_test.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True
createcellsBarrel.AuditExecute = True
createcellsEndcap.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [geantsim,
              createcellsBarrel,
              createcellsEndcap,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [podioevent, geoservice],
 )

