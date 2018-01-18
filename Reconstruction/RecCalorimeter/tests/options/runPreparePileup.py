from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

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

# common ECAL specific information
# readout name
ecalBarrelReadoutNamePhiEta = "ECalBarrelPhiEta"

from Configurables import TubeLayerPhiEtaCaloTool
ecalBarrelGeometry = TubeLayerPhiEtaCaloTool("EcalBarrelGeo",
                                             readoutName = ecalBarrelReadoutNamePhiEta,
                                             activeVolumeName = "LAr_sensitive",
                                             activeFieldName = "layer",
                                             fieldNames = ["system"],
                                             fieldValues = [5],
                                             activeVolumesNumber = 8)

from Configurables import PreparePileup
pileup = PreparePileup("PreparePileup",
                       geometryTool = ecalBarrelGeometry,
                       readoutName = ecalBarrelReadoutNamePhiEta,
                       layerFieldName = "layer",
                       numLayers = 8)
pileup.hits.Path="ECalBarrelCells"

THistSvc().Output = ["rec DATAFILE='histTest.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

out = PodioOutput("output", filename = "output_test.root",
                   OutputLevel = DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
pileup.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              pileup,
              out
              ],
    EvtSel = 'NONE',
    EvtMax = 1,
    ExtSvc = [podioevent, geoservice],
 )
