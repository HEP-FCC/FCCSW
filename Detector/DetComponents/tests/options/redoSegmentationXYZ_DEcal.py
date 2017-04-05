from Gaudi.Configuration import *
from Configurables import ApplicationMgr

from Configurables import FCCDataSvc
inputfile = "/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/batch/50Layers_2.1mmW_1mmAir_50umPixels_18umThick_FCCSW0.8pre/1PILEUPEVENTS_BFIELD1/output.root"
podiosvc   = FCCDataSvc("EventDataSvc", input=inputfile)

from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["positionedCaloHits"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Detector/DetFCChhECalDigital/compact/FCChh_DECalBarrel_Mockup.xml'
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
resegment.DataInputs.inhits.Path = "positionedCaloHits"
resegment.DataOutputs.outhits.Path = "newCaloHits"

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              doCellCalibration = False,
                              addCellNoise = False, filterCellNoise = False,
                              OutputLevel = DEBUG)
createcells.DataInputs.hits.Path="newCaloHits"
createcells.DataOutputs.cells.Path="newCaloCells"

from Configurables import FCCDataSvc, PodioOutput
#podiosvc = FCCDataSvc("EventDataSvc")
podioout = PodioOutput("out", filename="testResegmentationXYZ.root")
podioout.outputCommands = ["keep *"]

ApplicationMgr(EvtSel='NONE',
               EvtMax=5000,
               TopAlg=[podioinput, resegment, createcells, podioout],
               ExtSvc = [podiosvc, geoservice],
               OutputLevel=DEBUG)
