# Setup
# Names of cells collections
ecalBarrelCellsName = "ECalBarrelCells"
# Readouts
ecalBarrelReadoutName = "ECalBarrelPhiEta"

# Number of events
num_events = -1

from Gaudi.Configuration import *
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput
import sys

podioevent = FCCDataSvc("EventDataSvc")
import glob
podioevent.inputs=glob.glob("output_fullCalo_SimAndDigi_*.root")
# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader",
                        collections = [ecalBarrelCellsName,
                                       "GenParticles",
                                       ])


from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc")
# if FCC_DETECTORS is empty, this should use relative path to working directory
path_to_detector = os.environ.get("FCC_DETECTORS", "")
detectors_to_use=[
                    'Detector/DetFCCeeIDEA-LAr/compact/FCCee_DectMaster.xml'
                  ]
# prefix all xmls with path_to_detector
geoservice.detectors = [os.path.join(path_to_detector, _det) for _det in detectors_to_use]
geoservice.OutputLevel = WARNING


ecalBarrelNoisePath = "http://fccsw.web.cern.ch/fccsw/testsamples/elecNoise_ecalBarrelFCCee_50Ohm_traces1_4shieldWidth.root"
ecalBarrelNoiseHistName = "h_elecNoise_fcc_"

# add noise, create all existing cells in detector
from Configurables import NoiseCaloCellsFromFileTool
noiseBarrel = NoiseCaloCellsFromFileTool("NoiseBarrel",
                                         readoutName = ecalBarrelReadoutName,
                                         noiseFileName = ecalBarrelNoisePath,
                                         elecNoiseHistoName = ecalBarrelNoiseHistName,
                                         activeFieldName = "layer",
                                         addPileup = False,
                                         numRadialLayers = 8)
from Configurables import TubeLayerPhiEtaCaloTool
barrelGeometry = TubeLayerPhiEtaCaloTool("EcalBarrelGeo",
                                         readoutName = ecalBarrelReadoutName,
                                         activeVolumeName = "LAr_sensitive",
                                         activeFieldName = "layer",
                                         fieldNames = ["system"],
                                         fieldValues = [4],
                                         activeVolumesNumber = 8)
from Configurables import CreateCaloCells
createEcalBarrelCells = CreateCaloCells("CreateECalBarrelCells",
                                        geometryTool = barrelGeometry,
                                        doCellCalibration=False, # already calibrated
                                        addCellNoise=True, filterCellNoise=False,
                                        noiseTool = noiseBarrel,
                                        hits=ecalBarrelCellsName,
                                        cells=ecalBarrelCellsName+"Noise",
                                        OutputLevel=INFO)

#Empty cells for parts of calorimeter not implemented yet
from Configurables import CreateEmptyCaloCellsCollection
createemptycells =CreateEmptyCaloCellsCollection("CreateEmptyCaloCells")
createemptycells.cells.Path = "emptyCaloCells"

#Create calorimeter clusters
from GaudiKernel.PhysicalConstants import pi

from Configurables import CaloTowerTool
towers = CaloTowerTool("towers",
                               deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                               ecalBarrelReadoutName = ecalBarrelReadoutName,
                               ecalEndcapReadoutName = "",
                               ecalFwdReadoutName = "",
                               hcalBarrelReadoutName = "",
                               hcalExtBarrelReadoutName = "",
                               hcalEndcapReadoutName = "",
                               hcalFwdReadoutName = "",
                               OutputLevel = INFO)
towers.ecalBarrelCells.Path = ecalBarrelCellsName + "Noise"
towers.ecalEndcapCells.Path = "emptyCaloCells"
towers.ecalFwdCells.Path = "emptyCaloCells" 
towers.hcalBarrelCells.Path = "emptyCaloCells"
towers.hcalExtBarrelCells.Path = "emptyCaloCells"
towers.hcalEndcapCells.Path = "emptyCaloCells" 
towers.hcalFwdCells.Path = "emptyCaloCells"

# Cluster variables
windE = 9
windP = 17
posE = 5
posP = 11
dupE = 7
dupP = 13
finE = 9
finP = 17
# approx in GeV: changed from default of 12 in FCC-hh
threshold = 2

from Configurables import CreateCaloClustersSlidingWindow
createClusters = CreateCaloClustersSlidingWindow("CreateClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = windE, nPhiWindow = windP,
                                                 nEtaPosition = posE, nPhiPosition = posP,
                                                 nEtaDuplicates = dupE, nPhiDuplicates = dupP,
                                                 nEtaFinal = finE, nPhiFinal = finP,
                                                 energyThreshold = threshold)
createClusters.clusters.Path = "CaloClusters"

import uuid
out = PodioOutput("out", filename="output_allCalo_reco_noise_" + uuid.uuid4().hex + ".root")
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createClusters.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              createemptycells,
              createEcalBarrelCells,
              createClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = num_events,
    ExtSvc = [podioevent, geoservice])
