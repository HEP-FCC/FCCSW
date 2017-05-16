from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input = "output_ecalSim_e50GeV_1events.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections = ["ECalHits", "ECalPositionedHits"], OutputLevel = DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors = [  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml'],
                    OutputLevel = INFO)

# common ECAL specific information
# readout name
ecalReadoutName = "ECalHitsPhiEta"
# active material identifier name
ecalIdentifierName = "active_layer"
# active material volume name
ecalVolumeName = "LAr_sensitive"
# ECAL bitfield names & values
ecalFieldNames = ["system","ECAL_Cryo","bath","EM_barrel"]
ecalFieldValues = [5,1,1,1]

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool, NoiseCaloCellsFlatTool, TubeLayerPhiEtaCaloTool
calibcells = CalibrateCaloHitsTool("CalibrateCaloHitsTool", invSamplingFraction = "5.4")
noise = NoiseCaloCellsFlatTool("NoiseCaloCellsFlatTool", cellNoise = 0.01)
ecalgeo = TubeLayerPhiEtaCaloTool("EcalGeo",
                                  readoutName = ecalReadoutName,
                                  activeVolumeName = ecalVolumeName,
                                  activeFieldName = ecalIdentifierName,
                                  fieldNames = ecalFieldNames,
                                  fieldValues = ecalFieldValues,
                                  OutputLevel = DEBUG)

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              geometryTool = ecalgeo,
                              doCellCalibration = True,
                              calibTool = calibcells,
                              addCellNoise = True, filterCellNoise = False,
                              noiseTool = noise,
                              OutputLevel = DEBUG)
createcells.hits.Path = "ECalHits"
createcells.cells.Path = "caloCells"

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, SingleCaloTowerTool
from GaudiKernel.PhysicalConstants import pi
towers = SingleCaloTowerTool("towers",
                             deltaEtaTower = 0.01, deltaPhiTower = 2*pi/629.,
                             readoutName = ecalReadoutName,
                             OutputLevel = DEBUG)
towers.cells.Path = "caloCells"

createclusters = CreateCaloClustersSlidingWindow("CreateCaloClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = 7, nPhiWindow = 15,
                                                 nEtaPosition = 5, nPhiPosition = 11,
                                                 nEtaDuplicates = 5, nPhiDuplicates = 11,
                                                 nEtaFinal = 7, nPhiFinal = 15,
                                                 energyThreshold = 8,
                                                 OutputLevel = DEBUG)
createclusters.clusters.Path = "caloClusters"

out = PodioOutput("output", filename = "output_ecalReco_flatNoise_test.root",
                   OutputLevel = DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createclusters.AuditExecute = True
createcells.AuditExecute = True
out.AuditExecute = True


ApplicationMgr(
    TopAlg = [podioinput,
              createcells,
              createclusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax = 1,
    ExtSvc = [podioevent, geoservice],
 )
