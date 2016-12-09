import argparse
recoparser = argparse.ArgumentParser()
recoparser.add_argument('-t','--text', type=str, default=None, help='nocryo')
recoparser.add_argument('-b','--Bfield', type=int, help='1')
recoparser.add_argument('-e','--energy', type=int, help='10')
recoparser.add_argument('-eta','--winEta', type=int, help='')
recoparser.add_argument('-etapos','--winEtaPos', type=int, help='5')
recoparser.add_argument('-phi','--winPhi', type=int, help='3')
recoparser.add_argument('-phipos','--winPhiPos', type=int, help='15')
recoparser.add_argument('-p','--part', type=int, help='11')
recoargs, _ = recoparser.parse_known_args()

text = recoargs.text
B = str(recoargs.Bfield)
energy = recoargs.energy
winEta = recoargs.winEta
winEtaPos = recoargs.winEtaPos
winPhi = recoargs.winPhi
winPhiPos = recoargs.winPhiPos
part = recoargs.part

print "=================================="
print "geometry = ", text
print "B = ", B
print "energy = ", energy
print "part = ", part
print "reco window  (eta) ", winEta, " - ", winEtaPos, " (phi) ", winPhi, " - ", winPhiPos
print "Reconstructing ...  /eos/fcc/users/n/novaj/newgeometry_gev_15nov/output_e"+str(energy)+"GeV_etaMin-1.5_etaMax1.5_bfield"+B+"_part"+str(part)+"_Lar4mm_Pb2mm_"+text+"_v0.root"
print "=================================="

configreadout = "ECalHitsPhiEta"
configsensitiveField = "active_layer"
configsensitiveVolume = "LAr_sensitive"
configfieldNames = ["system","ECAL_Cryo","bath","EM_barrel","active_layer"]
if text == 'nocryo':
    configfieldVals = [5,0,0,1,0]
elif text == 'tracker_cryo':
    configfieldVals = [5,1,1,1,0]

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input="root://eospublic.cern.ch//eos/fcc/users/n/novaj/newgeometry_gev_15nov/output_e"+str(energy)+"GeV_etaMin-1.5_etaMax1.5_bfield"+B+"_part"+str(part)+"_Lar4mm_Pb2mm_"+text+"_v0.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader",  collections=["ECalHits", "ECalPositionedHits"], OutputLevel=INFO)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_'+text+'.xml'],
                    OutputLevel = INFO)

from Configurables import RedoSegmentation
resegment = RedoSegmentation("ReSegmentation",
                             # old bitfield (readout)
                             oldReadoutName = "deprecatedECalHitsPhiEta",
                             # # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = ["phi"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = configreadout,
                             OutputLevel = INFO)
# clusters are needed, with deposit position and cellID in bits
resegment.DataInputs.inhits.Path = "ECalPositionedHits"
resegment.DataOutputs.outhits.Path = "newECalHits"

#Configure tools for calo reconstruction
#Create calo cells
from Configurables import CalibrateCaloHitsTool
calibcells = CalibrateCaloHitsTool("CalibrateCaloHitsTool",invSamplingFraction="5.4")

from Configurables import CreateCaloCells
createcells = CreateCaloCells("CreateCaloCells",
                              calibTool=calibcells, doCellCalibration=True,
                              addCellNoise=True, filterCellNoise=False,
                              activeVolumeName=configsensitiveVolume,
                              activeFieldName=configsensitiveField,
                              readoutName=configreadout,
                              fieldNames=configfieldNames,
                              fieldValues=configfieldVals,
                              OutputLevel=INFO)
createcells.DataInputs.hits.Path="newECalHits"
createcells.DataOutputs.cells.Path="caloCells"

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow
from GaudiKernel.PhysicalConstants import pi
createclusters = CreateCaloClustersSlidingWindow("CreateCaloClusters",
                                                 readoutName = configreadout,
                                                 fieldNames = configfieldNames,
                                                 fieldValues = configfieldVals,
                                                 deltaEtaTower=0.01, deltaPhiTower=2*pi/629.,
                                                 nEtaWindow = winEta, nPhiWindow = winPhi,
                                                 nEtaPosition = winEtaPos, nPhiPosition = winPhiPos,
                                                 nEtaDuplicates = winEta, nPhiDuplicates = winPhi,
                                                 energyThreshold = 3,
                                                 OutputLevel=INFO)
createclusters.DataInputs.cells.Path="caloCells"
createclusters.DataOutputs.clusters.Path="caloClusters"

# from Configurables import CreatePositionedHit
# positionhit = CreatePositionedHit("CreatePositionedHit",
#                                   readoutName = configreadout,
#                                   activeFieldName = configsensitiveField,
#                                   activeVolumeName=configsensitiveVolume,
#                                   OutputLevel=INFO)
# positionhit.DataInputs.caloCells.Path="caloCells"
# positionhit.DataOutputs.caloPositionedHits.Path="caloCellsPositions"


out = PodioOutput("out", filename="dataEosDec16/root/reco_e"+str(energy)+"GeV_etaMin-1.5_etaMax1.5_bfield"+B+"_part"+str(part)+"_Lar4mm_Pb2mm_"+text+"_v0_winEta"+str(winEta)+str(winEtaPos)+str(winEta)+"_winPhi"+str(winPhi)+str(winPhiPos)+str(winPhi)+"_en3.root",
                   OutputLevel=INFO)
out.outputCommands = ["drop *", "keep caloClusters"] #, "keep caloCells"] #, "keep caloCellsPositions"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
createclusters.AuditExecute = True
resegment.AuditExecute = True
createcells.AuditExecute = True
# positionhit.AuditExecute = True
out.AuditExecute = True


ApplicationMgr(
    TopAlg = [podioinput,
              resegment,
              createcells,
              createclusters,
              # positionhit,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 500,
    ExtSvc = [podioevent, geoservice],
 )

