from Gaudi.Configuration import *

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'
                                        ],
                    OutputLevel = INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import CreateFCChhCaloNeighbours
neighbours = CreateFCChhCaloNeighbours("neighbours", 
                                       outputFileName="neighbours_map_barrel.root",
                                       readoutNamesPhiEta=["ECalBarrelPhiEta", "BarHCal_Readout_phieta"], 
                                       systemNamesPhiEta=["system","system"],
                                       systemValuesPhiEta=[5,8],
                                       activeFieldNamesPhiEta=["layer","layer"],
                                       activeVolumesNumbers=[8,10],
                                       activeVolumesEta = [1.2524, 1.2234, 1.1956, 1.1561, 1.1189, 1.0839, 1.0509, 0.9999, 0.9534, 0.91072],
                                       readoutNamesVolumes=[],
                                       connectBarrels=True, 
                                       OutputLevel=INFO)

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [geoservice, neighbours],
                OutputLevel=INFO
)
