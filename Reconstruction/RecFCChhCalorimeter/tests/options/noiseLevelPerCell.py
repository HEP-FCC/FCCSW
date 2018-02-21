from Gaudi.Configuration import *

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'
                                        ],
                    OutputLevel = INFO)

ecalBarrelReadoutName = "ECalBarrelPhiEta"
# noise files
ecalBarrelNoisePath = "/afs/cern.ch/user/a/azaborow/public/FCCSW/elecNoise_ecalBarrel_50Ohm_traces2_2shieldWidth_noise.root"
#"/afs/cern.ch/user/n/novaj/public/elecNoise_sfcorrection_50Ohm_default_differentTraces_168.root"
ecalBarrelNoiseHistName = "h_elecNoise_fcc_"

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import CreateFCChhCaloNoiseLevelMap, ConstNoiseTool,  ReadNoiseFromFileTool

ECalNoiseTool = ReadNoiseFromFileTool("ReadNoiseFromFileToolECal",  
                                      readoutName = ecalBarrelReadoutName,
                                      noiseFileName = ecalBarrelNoisePath,
                                      elecNoiseHistoName = ecalBarrelNoiseHistName,
                                      activeFieldName = "layer",
                                      addPileup = False,
                                      numRadialLayers = 8,
                                      OutputLevel=DEBUG)

HCalNoiseTool = ConstNoiseTool("ConstNoiseTool", noiseInHCalBarrel = 0.0009)

noisePerCell = CreateFCChhCaloNoiseLevelMap("noisePerCell", 
                                          ECalBarrelNoiseTool = ECalNoiseTool, 
                                          HCalBarrelNoiseTool = HCalNoiseTool,
                                          OutputLevel=DEBUG)

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [geoservice, noisePerCell],
                OutputLevel=INFO
)
