from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

# v01 production - min. bias events
<<<<<<< HEAD
podioevent = FCCDataSvc("EventDataSvc", input="http://fccsw.web.cern.ch/fccsw/testsamples/caloCells_minBiasEvent.root")
=======
podioevent = FCCDataSvc("EventDataSvc", input="/eos/experiment/fcc/hh/simulation/samples/v01/physics/MinBias/bFieldOn/etaFull/simu/output_condor_novaj_201801081606412074.root")
>>>>>>> 4cbd4d807ac0e514f39f8b49fec8b9cb130500d2

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput

<<<<<<< HEAD
podioinput = PodioInput("in", collections = ["ECalBarrelCells"])

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors = [ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                  'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml'],
                    OutputLevel = WARNING)
=======
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
                  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                  'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                  'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
                  'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                  'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
                  'file:Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml',
                  'file:Detector/DetFCChhTailCatcher/compact/FCChh_TailCatcher.xml',
                  'file:Detector/DetFCChhBaseline1/compact/FCChh_Solenoids.xml',
                  'file:Detector/DetFCChhBaseline1/compact/FCChh_Shielding.xml' ],
                    OutputLevel = DEBUG)
>>>>>>> 4cbd4d807ac0e514f39f8b49fec8b9cb130500d2

# Pileup in ECal Barrel
# readout name
ecalBarrelReadoutNamePhiEta = "ECalBarrelPhiEta"
<<<<<<< HEAD
=======
hcalBarrelReadoutNamePhiEta = "BarHCal_Readout_phieta"
>>>>>>> 4cbd4d807ac0e514f39f8b49fec8b9cb130500d2

# geometry tool
from Configurables import TubeLayerPhiEtaCaloTool
ecalBarrelGeometry = TubeLayerPhiEtaCaloTool("EcalBarrelGeo",
                                             readoutName = ecalBarrelReadoutNamePhiEta,
                                             activeVolumeName = "LAr_sensitive",
                                             activeFieldName = "layer",
                                             fieldNames = ["system"],
                                             fieldValues = [5],
                                             activeVolumesNumber = 8)

<<<<<<< HEAD
from Configurables import CreateEmptyCaloCellsCollection
createemptycells = CreateEmptyCaloCellsCollection("CreateEmptyCaloCells")
createemptycells.cells.Path = "emptyCaloCells"
#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, CaloTowerTool
from GaudiKernel.PhysicalConstants import pi

towersE = CaloTowerTool("towers",
                               deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                               ecalBarrelReadoutName = ecalBarrelReadoutNamePhiEta,
                               ecalEndcapReadoutName = "",
                               ecalFwdReadoutName = "",
                               hcalBarrelReadoutName = "",
                               hcalExtBarrelReadoutName = "",
                               hcalEndcapReadoutName = "",
                               hcalFwdReadoutName = "")
towersE.ecalBarrelCells.Path = "ECalBarrelCells"
towersE.ecalEndcapCells.Path = "emptyCaloCells"
towersE.ecalFwdCells.Path = "emptyCaloCells"
towersE.hcalBarrelCells.Path = "emptyCaloCells"
towersE.hcalExtBarrelCells.Path = "emptyCaloCells"
towersE.hcalEndcapCells.Path = "emptyCaloCells"
towersE.hcalFwdCells.Path = "emptyCaloCells"
=======
from Configurables import TubeLayerPhiEtaCaloTool
hcalBarrelGeometry = TubeLayerPhiEtaCaloTool("HcalBarrelGeo",
                                             readoutName = hcalBarrelReadoutNamePhiEta,
                                             activeVolumeName = "layer",
                                             activeFieldName = "layer",
                                             fieldNames = ["system"],
                                             fieldValues = [8],
                                             activeVolumesNumber = 10)
>>>>>>> 4cbd4d807ac0e514f39f8b49fec8b9cb130500d2

# call pileup tool
# prepare TH2 histogram with pileup per abs(eta)
from Configurables import PreparePileup
pileupEcalBarrel = PreparePileup("PreparePileupEcalBarrel",
<<<<<<< HEAD
                                 geometryTool = ecalBarrelGeometry,
                                 readoutName = ecalBarrelReadoutNamePhiEta,
                                 layerFieldName = "layer",
                                 towerTool = towersE,
                                 histogramName = "ecalBarrelEnergyVsAbsEta",
                                 etaSize = [7],
                                 phiSize = [19],
                                 numLayers = 8)
pileupEcalBarrel.hits.Path="ECalBarrelCells"

=======
                       geometryTool = ecalBarrelGeometry,
                       readoutName = ecalBarrelReadoutNamePhiEta,
                       layerFieldName = "layer",
                       histogramName = "ecalBarrelEnergyVsAbsEta",
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

>>>>>>> 4cbd4d807ac0e514f39f8b49fec8b9cb130500d2
THistSvc().Output = ["rec DATAFILE='pileupCalBarrel.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

<<<<<<< HEAD
ApplicationMgr(
    TopAlg = [podioinput,
              createemptycells,
              pileupEcalBarrel,
              ],
    EvtSel = 'NONE',
    OutputLevel = DEBUG,
    EvtMax = 5,
=======
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
              pileupEcalBarrel,
              pileupHcalBarrel,
              ],
    EvtSel = 'NONE',
    EvtMax = 10,
>>>>>>> 4cbd4d807ac0e514f39f8b49fec8b9cb130500d2
    ExtSvc = [podioevent, geoservice],
 )
