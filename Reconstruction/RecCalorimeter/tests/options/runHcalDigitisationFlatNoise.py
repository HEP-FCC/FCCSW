from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input="output_hcalSim_e50GeV_eta036_1events.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["HCalHits"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)

# common HCAL specific information
# readout name
hcalReadoutName = "BarHCal_Readout"
# active material identifier name
hcalIdentifierName = ["module", "row", "layer", "tile"]
# active material volume name
hcalVolumeName = ["moduleVolume", "wedgeVolume", "layerVolume", "modCompVolume"]
# ECAL bitfield names & values
hcalFieldNames=["system"]
hcalFieldValues=[8]

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool, NoiseCaloCellsFlatTool, NestedVolumesCaloTool
noise = NoiseCaloCellsFlatTool("HCalNoise")
hcalgeo = NestedVolumesCaloTool("HcalGeo",
                                 activeVolumeName = hcalVolumeName,
                                 activeFieldName = hcalIdentifierName,
                                 readoutName = hcalReadoutName,
                                 fieldNames = hcalFieldNames,
                                 fieldValues = hcalFieldValues,
                                 OutputLevel = DEBUG)

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              geometryTool = hcalgeo,
                              doCellCalibration = False,
                              addCellNoise = True, filterCellNoise = False,
                              noiseTool = noise,
                              OutputLevel = DEBUG)
createcells.hits.Path="HCalHits"
createcells.cells.Path="HCalCells"

out = PodioOutput("out", filename="output_HCalCells_digitisation_allCells.root",
                   OutputLevel = DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createcells.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              createcells,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [podioevent, geoservice],
 )

