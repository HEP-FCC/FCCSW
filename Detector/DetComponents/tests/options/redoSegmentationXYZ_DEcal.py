from Gaudi.Configuration import *
from Configurables import ApplicationMgr

from Configurables import FCCDataSvc
inputfile = "/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/batch_eos/50Layers_2.1mmW_50umPixels_18umThick_FCCSW0.8/100GeV_BFIELD4T_ETAMIN-0.001_ETAMAX0.001/output_100GeV_BFIELD4T_1.root"
podiosvc   = FCCDataSvc("EventDataSvc", input=inputfile)

from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["positionedCaloHits"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Detector/DetFCChhECalDigital/compact/FCChh_DECalBarrel_50Layers_2.1mmW_50umPixels_18umThick.xml'
                                        ],
                    OutputLevel = DEBUG)

from Configurables import RedoSegmentation
resegment = RedoSegmentation("ReSegmentation",
                             # old bitfield (readout)
                             oldReadoutName = "BarDECal_Readout",
                             # specify which fields are going to be deleted
                             oldSegmentationIds = ["x","y","z"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName="BarDECal_Pads",
                             OutputLevel = DEBUG)
# clusters are needed, with deposit position and cellID in bits
resegment.inhits.Path = "positionedCaloHits"
resegment.outhits.Path = "newCaloHits"

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              doCellCalibration = False,
                              addCellNoise = False, filterCellNoise = False,
                              OutputLevel = DEBUG)
createcells.hits.Path="newCaloHits"
createcells.cells.Path="newCaloCells"

from Configurables import DECalLongitudinalTest
hist = DECalLongitudinalTest("hists",   
                                 readoutName = "BarDECal_Readout",
                                 layerFieldName = "layer",
                                 numLayers = 50, # one more because index starts at 1 - layer 0 will be always empty
                                 OutputLevel = DEBUG)
hist.deposits.Path="positionedCaloHits"

THistSvc().Output = ["rec DATAFILE='./hist_test.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=False
THistSvc().OutputLevel=INFO

from Configurables import FCCDataSvc, PodioOutput
#podiosvc = FCCDataSvc("EventDataSvc")
podioout = PodioOutput("out", filename="/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/batch_eos/50Layers_2.1mmW_50umPixels_18umThick_FCCSW0.8/100GeV_BFIELD4T_ETAMIN-0.001_ETAMAX0.001/testResegmentationXYZ_100GeV.root")
podioout.outputCommands = ["keep *"]

ApplicationMgr(EvtSel='NONE',
               EvtMax=5000,
               TopAlg=[podioinput, resegment, createcells,hist, podioout],
               ExtSvc = [podiosvc, geoservice],
               OutputLevel=DEBUG)
