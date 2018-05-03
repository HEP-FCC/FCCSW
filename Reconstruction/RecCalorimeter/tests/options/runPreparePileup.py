from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

# v01 production - min. bias events
podioevent = FCCDataSvc("EventDataSvc", input="/eos/experiment/fcc/hh/simulation/samples/v01/physics/MinBias/bFieldOn/etaFull/simu/output_condor_novaj_201801081606412074.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput

podioinput = PodioInput("in", collections = ["ECalBarrelCells",
                                             "ECalEndcapCells",
                                             "ECalFwdCells",
                                             "HCalBarrelCells",
                                             "HCalExtBarrelCells",
                                             "HCalEndcapCells",
                                             "HCalFwdCells",
                                             "TailCatcherCells"])

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors = [ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                  # 'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                  'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                  # 'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
                  # 'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                  # 'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
                  # 'file:Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml',
                  # 'file:Detector/DetFCChhTailCatcher/compact/FCChh_TailCatcher.xml',
                  # 'file:Detector/DetFCChhBaseline1/compact/FCChh_Solenoids.xml',
                  # 'file:Detector/DetFCChhBaseline1/compact/FCChh_Shielding.xml'
],
                    OutputLevel = WARNING)

# Pileup in ECal Barrel
# readout name
ecalBarrelReadoutNamePhiEta = "ECalBarrelPhiEta"
hcalBarrelReadoutNamePhiEta = "BarHCal_Readout_phieta"

# geometry tool
from Configurables import TubeLayerPhiEtaCaloTool
ecalBarrelGeometry = TubeLayerPhiEtaCaloTool("EcalBarrelGeo",
                                             readoutName = ecalBarrelReadoutNamePhiEta,
                                             activeVolumeName = "LAr_sensitive",
                                             activeFieldName = "layer",
                                             fieldNames = ["system"],
                                             fieldValues = [5],
                                             activeVolumesNumber = 8)

from Configurables import TubeLayerPhiEtaCaloTool
hcalBarrelGeometry = TubeLayerPhiEtaCaloTool("HcalBarrelGeo",
                                             readoutName = hcalBarrelReadoutNamePhiEta,
                                             activeVolumeName = "layer",
                                             activeFieldName = "layer",
                                             fieldNames = ["system"],
                                             fieldValues = [8],
                                             activeVolumesNumber = 10)


from Configurables import CreateEmptyCaloCellsCollection
createemptycells = CreateEmptyCaloCellsCollection("CreateEmptyCaloCells")
createemptycells.cells.Path = "emptyCaloCells"
#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, CaloTowerTool
from GaudiKernel.PhysicalConstants import pi

towers = CaloTowerTool("towers",
                               deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                               ecalBarrelReadoutName = ecalBarrelReadoutNamePhiEta,
                               ecalEndcapReadoutName = "",
                               ecalFwdReadoutName = "",
                               hcalBarrelReadoutName = "",
                               hcalExtBarrelReadoutName = "",
                               hcalEndcapReadoutName = "",
                               hcalFwdReadoutName = "")
towers.ecalBarrelCells.Path = "ECalBarrelCells"
towers.ecalEndcapCells.Path = "emptyCaloCells"
towers.ecalFwdCells.Path = "emptyCaloCells"
towers.hcalBarrelCells.Path = "emptyCaloCells"
towers.hcalExtBarrelCells.Path = "emptyCaloCells"
towers.hcalEndcapCells.Path = "emptyCaloCells"
towers.hcalFwdCells.Path = "emptyCaloCells"

# call pileup tool
# prepare TH2 histogram with pileup per abs(eta)
from Configurables import PreparePileup
pileupEcalBarrel = PreparePileup("PreparePileupEcalBarrel",
                                 geometryTool = ecalBarrelGeometry,
                                 readoutName = ecalBarrelReadoutNamePhiEta,
                                 layerFieldName = "layer",
                                 towerTool = towers,
                                 histogramName = "ecalBarrelEnergyVsAbsEta",
                                 etaSize = [1],
                                 phiSize = [1],
                                 numLayers = 8)
pileupEcalBarrel.hits.Path="ECalBarrelCells"

from Configurables import PreparePileup
pileupHcalBarrel = PreparePileup("PreparePileupHcalBarrel",
                       geometryTool = hcalBarrelGeometry,
                       readoutName = hcalBarrelReadoutNamePhiEta,
                       layerFieldName = "layer",
                       histogramName ="hcalBarrelEnergyVsAbsEta",
                       numLayers = 10)
pileupHcalBarrel.hits.Path="HCalBarrelCells"

THistSvc().Output = ["rec DATAFILE='pileupCalBarrel.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

#out = PodioOutput("output", filename = "output.root",
#                   OutputLevel = DEBUG)
#out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
#pileupEcalBarrel.AuditExecute = True
#out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              createemptycells,
              pileupEcalBarrel,
              # pileupHcalBarrel,
              ],
    EvtSel = 'NONE',
    EvtMax = -1,
    ExtSvc = [podioevent, geoservice],
 )
