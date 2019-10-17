from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input="output_hcalSim_e50GeV_eta036_10events.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["HCalHits"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)

# common HCAL specific information
# readout name
hcalReadoutName = "HCalBarrelReadout"
# active material identifier name
hcalIdentifierName = ["module", "row", "layer"]
# active material volume name
hcalVolumeName = ["moduleVolume", "wedgeVolume", "layerVolume"]
# ECAL bitfield names & values
hcalFieldNames=["system"]
hcalFieldValues=[8]

#Configure tools for calo reconstruction
from Configurables import RewriteBitfield, CalibrateCaloHitsTool, NoiseCaloCellsFlatTool, LayerPhiEtaCaloTool
calibHcells = CalibrateCaloHitsTool("CalibrateHCal", invSamplingFraction="41.7 ")
noise = NoiseCaloCellsFlatTool("HCalNoise",
                               cellNoise = 0.01)

rewriteHCal = RewriteBitfield("RewriteHCal",
                                # old bitfield (readout)
                                oldReadoutName = "HCalBarrelReadout",
                                # specify which fields are going to be deleted
                                removeIds = ["row"],
                                # new bitfield (readout), with new segmentation
                                newReadoutName = "BarHCal_Readout_phieta",
                                debugPrint = 10,
                                OutputLevel= INFO)
# clusters are needed, with deposit position and cellID in bits
rewriteHCal.inhits.Path = "HCalHits"
rewriteHCal.outhits.Path = "HCalBarrelCellsStep1"

# Geometry for layer-eta-phi segmentation
barrelHcalGeometry = LayerPhiEtaCaloTool("BarrelHcalGeo",
                                         readoutName = "BarHCal_Readout_phieta",
                                         activeVolumeName = "layerVolume",
                                         activeFieldName = "layer",
                                         fieldNames = ["system"],
                                         fieldValues = [8],
                                         activeVolumesNumber = 10,
                                         activeVolumesEta = [1.2524, 1.2234, 1.1956, 1.15609, 1.1189, 1.08397, 1.0509, 0.9999, 0.9534, 0.91072],
                                         OutputLevel= DEBUG)

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              geometryTool = barrelHcalGeometry,
                              calibTool=calibHcells,
                              doCellCalibration = True,
                              addCellNoise = True, filterCellNoise = False,
                              noiseTool = noise,
                              OutputLevel = DEBUG)
createcells.hits.Path="HCalBarrelCellsStep1"
createcells.cells.Path="HCalCells"

out = PodioOutput("out", filename="output_HCalCells_digitisation_allCells.root",
                   OutputLevel = DEBUG)
out.outputCommands = ["keep *", "drop HCalBarrelCellsStep1"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
rewriteHCal.AuditExecute = True
createcells.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              rewriteHCal,
              createcells,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [podioevent, geoservice],
 )

