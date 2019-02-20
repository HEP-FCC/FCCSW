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

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool, CreateVolumeCaloPositions, RedoSegmentation, RewriteBitfield
calibHcells = CalibrateCaloHitsTool("CalibrateHCal", invSamplingFraction="41.7 ")

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
rewriteHCal.outhits.Path = "HCalBarrelCellsStep2"

# Use Phi-Eta segmentation in Hcal barrel
resegmentHcalBarrel = RewriteBitfield("ReSegmentationHcal",
                                       # old bitfield (readout)                         
                                       oldReadoutName = "HCalBarrelReadout",
                                       # specify which fields are going to be altered (deleted/rewritten)
                                       removeIds = ["eta"],
                                       # new bitfield (readout), with new segmentation                   
                                       newReadoutName = "BarHCal_Readout_phi",
                                       inhits = "HCalHits",
                                       outhits = "HCalBarrelCellsStep1")

from Configurables import CreateCaloCells
createHcalBarrelTiles = CreateCaloCells("CreateHCalBarrelTiles",
                                        calibTool=calibHcells,
                                        doCellCalibration=True,
                                        addCellNoise=False, filterCellNoise=False,
                                        OutputLevel=INFO,
                                        hits="HCalBarrelCellsStep1",
                                        cells="HCalAllTiles")

createHcalBarrelCells = CreateCaloCells("CreateHCalBarrelCells",
                                        calibTool=calibHcells,
                                        doCellCalibration=True,
                                        addCellNoise=False, filterCellNoise=False,
                                        OutputLevel=INFO,
                                        hits="HCalBarrelCellsStep2",
                                        cells="HCalCells")

out = PodioOutput("out", filename="output_HCalCells_digitisation_noNoise.root",
                   OutputLevel = DEBUG)
out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createHcalBarrelCells.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              resegmentHcalBarrel,
              rewriteHCal,
              createHcalBarrelTiles,
              createHcalBarrelCells,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [podioevent, geoservice],
 )

