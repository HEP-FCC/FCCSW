from Gaudi.Configuration import *

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'
                                        ],
                    OutputLevel = INFO)

ecalBarrelReadoutName = "ECalBarrelPhiEta"
hcalBarrelReadoutName = "HCalBarrelReadout"
# noise files
BarrelNoisePath = "/afs/cern.ch/work/c/cneubuse/public/FCChh/inBfield/noiseBarrel_mu100.root"
ecalBarrelNoiseHistName = "h_elecNoise_fcc_"
hcalBarrelNoiseHistName = "h_elec_hcal_layer"

from Configurables import CellPositionsECalBarrelTool, CellPositionsHCalBarrelNoSegTool
ECalBcells = CellPositionsECalBarrelTool("CellPositionsECalBarrel", 
                                         readoutName = ecalBarrelReadoutName, 
                                         OutputLevel = INFO)
HCalBcellVols = CellPositionsHCalBarrelNoSegTool("CellPositionsHCalBarrelVols", 
                                                 readoutName = hcalBarrelReadoutName, 
                                                 OutputLevel = INFO)
# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import CreateFCChhCaloNoiseLevelMap, ConstNoiseTool,  ReadNoiseFromFileTool

ECalNoiseTool = ReadNoiseFromFileTool("ReadNoiseFromFileToolECal",  
                                      readoutName = ecalBarrelReadoutName,
                                      positionsTool = ECalBcells,
                                      noiseFileName = BarrelNoisePath,
                                      elecNoiseHistoName = ecalBarrelNoiseHistName,
                                      setNoiseOffset = False,
                                      activeFieldName = "layer",
                                      addPileup = False,
                                      numRadialLayers = 8,
                                      OutputLevel=DEBUG)

HCalNoiseTool = ReadNoiseFromFileTool("ReadNoiseFromFileToolHCal",  
                                      readoutName = hcalBarrelReadoutName,
                                      positionsTool = HCalBcellVols,
                                      noiseFileName = BarrelNoisePath,
                                      elecNoiseHistoName = hcalBarrelNoiseHistName,
                                      setNoiseOffset = False,
                                      activeFieldName = "layer",
                                      addPileup = False,
                                      numRadialLayers = 10,
                                      OutputLevel=DEBUG)

noisePerCell = CreateFCChhCaloNoiseLevelMap("noisePerCell", 
                                            ECalBarrelNoiseTool = ECalNoiseTool, 
                                            HCalBarrelNoiseTool = HCalNoiseTool,
                                            outputFileName="cellNoise_Barrel.root",
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
