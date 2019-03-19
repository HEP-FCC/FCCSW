from Gaudi.Configuration import *

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'
                                        ],
                    OutputLevel = INFO)

ecalBarrelReadoutName = "ECalBarrelPhiEta"
hcalBarrelReadoutName = "BarHCal_Readout_phieta"
# noise files
# has HCal electr. noise levels as well as ECal electr. noise level
BarrelNoisePath = "/afs/cern.ch/work/c/cneubuse/public/FCChh/inBfield/noiseBarrel_PU200.root"
ecalBarrelNoiseHistName = "h_elecNoise_fcc_"
hcalBarrelNoiseHistName = "h_elec_hcal_layer"

from Configurables import CellPositionsECalBarrelTool, CellPositionsHCalBarrelTool
# ATTENTION!
# The parameters have to be default in the tools, problem in Gaudi does not propagate the options through 2 tools
ECalBcells = CellPositionsECalBarrelTool("CellPositionsECalBarrel", 
                                         readoutName = ecalBarrelReadoutName, 
                                         OutputLevel = INFO)
HCalBcells = CellPositionsHCalBarrelTool("CellPositionsHCalBarrel", 
                                         readoutName = hcalBarrelReadoutName, 
                                         radii = [291.05, 301.05, 313.55, 328.55, 343.55, 358.55, 378.55, 403.55, 428.55, 453.55],
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
                                      positionsTool = HCalBcells,
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
                                            readoutNamesPhiEta=["ECalBarrelPhiEta", "BarHCal_Readout_phieta"],
                                            systemNamesPhiEta=["system","system"],
                                            systemValuesPhiEta=[5,8],
                                            activeFieldNamesPhiEta=["layer","layer"],
                                            activeVolumesNumbers=[8,10],
                                            activeVolumesEta = [1.2524, 1.2234, 1.1956, 1.1561, 1.1189, 1.0839, 1.0509, 0.9999, 0.9534, 0.91072],
                                            readoutNamesVolumes=[],
                                            outputFileName="/afs/cern.ch/work/c/cneubuse/public/FCChh/cellNoise_map_electronicsNoiseLevel.root",
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
