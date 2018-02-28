from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
batch_dir = "/eos/user/t/toprice/private/FCC/FCCSW/v8.0/XYZ/"
fccsw_version = "FCCSW0.8"
det_config = "50Layers_2.1mmW_FCCSW0.8"
run_config = "100GeV_BFIELD4T_ETAMIN-0.001_ETAMAX0.001"
file = "100GeV_BFIELD4T_0.root"
#inputfile = batch_dir+"/"+det_config+"_"+fccsw_version+"/"+run_config+"/"+file
inputfile = batch_dir+"/"+det_config+"/"+run_config+"/output_"+file
podiosvc   = FCCDataSvc("EventDataSvc", input=inputfile)


from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["positionedCaloHits", "GenParticles"], OutputLevel=INFO)

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/Detector/DetFCChhECalDigital/compact/FCChh_DECalBarrel_'+det_config[:-9]+'.xml'
],
                    OutputLevel = INFO)

from Configurables import FilterSiliconEcalHits
filtered = FilterSiliconEcalHits("FilterSiEcal",
                            readoutName = "BarDECal_Readout",
                            digitalFlag = 1)
filtered.deposits.Path="positionedCaloHits"
filtered.filtered.Path="filteredCaloHits"                        

# add a processor which generates noise hits
# input - noise level
#       - threshold level
#       - segmentation name

# add a processor which adds noise event in to genuine hits
# take input of noise hits and genuine hits
# output a combination for the event

from Configurables import RedoSegmentation
resegment = RedoSegmentation("ReSegmentation",
                             # old bitfield (readout)
                             oldReadoutName = "BarDECal_Readout",
                             # specify which fields are going to be deleted
                             oldSegmentationIds = ["x","y","z"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName="BarDECal_Pads",
                             OutputLevel = INFO)
# clusters are needed, with deposit position and cellID in bits
resegment.inhits.Path = "filteredCaloHits"
resegment.outhits.Path = "newCaloHits"

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              doCellCalibration = False,
                              addCellNoise = False, filterCellNoise = False, sumPixelsPerCell = False,
                              OutputLevel = INFO)
createcells.hits.Path="newCaloHits"
createcells.cells.Path="newCaloCells"

from Configurables import DECalAnalysis
hist = DECalAnalysis("DECalAnalysis",   
                                 pixelReadoutName = "BarDECal_Readout",
                                 padReadoutName = "BarDECal_Pads",
                                 layerFieldName = "layer",
                                 numLayers = 50, # one more because index starts at 1 - layer 0 will be always empty
                                 OutputLevel = INFO)
hist.pixels.Path="filteredCaloHits"
hist.pads.Path="newCaloCells"
hist.truth.Path="GenParticles"

#THistSvc().Output = ["rec DATAFILE='"+batch_dir+"/"+det_config+"_"+fccsw_version+"/"+run_config+"/<OUTPUT>' TYP='ROOT' OPT='RECREATE'"]
THistSvc().Output = ["rec DATAFILE='"+batch_dir+"/"+det_config+"/"+run_config+"/digital_"+file+"' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=False
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
hist.AuditExecute = True

from Configurables import FCCDataSvc, PodioOutput
#podiosvc = FCCDataSvc("EventDataSvc")
podioout = PodioOutput("out", filename=batch_dir+"/"+det_config+"/"+run_config+"/redoSegmentation_"+file)
podioout.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [podioinput, filtered, resegment, createcells,hist, podioout],
                EvtSel = 'NONE',
                EvtMax = 10,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podiosvc,geoservice, audsvc],
                OutputLevel = INFO
)
