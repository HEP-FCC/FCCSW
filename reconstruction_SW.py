import os
import numpy as np

#loads array of random seeds from file                                                                                                                                                                                                   
#seed_array = np.loadtxt('/afs/cern.ch/user/c/cneubuse/FCCSW/condor/seeds.txt',dtype='int',delimiter=',')                                                                                                                                   
#set these in the .sh script                                                                                                                                                                                                                

energy=50
num_events=1
bfield=0
i=1
particle=3
eta=0.36

particleType = "pi-"
if particle==0:
    particleType = "e-"
if particle==2:
    particleType = "mu-"
if particle==3:
    particleType = "pi0"
print particleType

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input="root://eospublic//eos/fcc/users/c/cneubuse/FccHcal/fullModelSim/combCalo/output_combCalo_pi0"+str(energy)+"GeV_bfield"+str(bfield)+"_eta"+str(eta)+"_part"+str(i)+".root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections = ["ECalPositions", "newHCalCells", "GenParticles", "GenVertices"], OutputLevel = DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)

# common ECAL specific information
# readout name
ecalReadoutName = "ECalHitsEta"
ecalReadoutNameOffset = "ECalHitsPhiEta"
# active material identifier name
ecalIdentifierName = "active_layer"
# active material volume name
ecalVolumeName = "LAr_sensitive"
# ECAL bitfield names & values
ecalFieldNames=["system","ECAL_Cryo","bath","EM_barrel"]
ecalFieldValues=[5,1,1,1]
# common HCAL specific information
# active material identifier name
#hcalIdentifierName = ["module","row","layer","tile"]
# active material volume name
#hcalVolumeName = ["module","wedge","layer","tile"]
# HCAL bitfield names & values
#hcalFieldNames=["system"]
# readout name
hcalReadoutNameOffset = "BarHCal_Readout_phieta"

# Configure tools for calo reconstruction                                                                                                

from Configurables import RedoSegmentation
resegmentEcal = RedoSegmentation("ReSegmentationEcal",
                             # old bitfield (readout)
                             oldReadoutName = ecalReadoutName,
                             # # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = ["module"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = ecalReadoutNameOffset,
                             OutputLevel = INFO,
                             inhits = "ECalPositions",
                             outhits = "ECalCellsForSW")


#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, SingleCaloTowerTool
from GaudiKernel.PhysicalConstants import pi
towersEcal = SingleCaloTowerTool("towersEcal",
                             deltaEtaTower = 0.01, deltaPhiTower = 2*pi/512.,
                             readoutName = ecalReadoutNameOffset,
                             OutputLevel = DEBUG)
towersEcal.cells.Path = "ECalCellsForSW"


createEcalClusters = CreateCaloClustersSlidingWindow("CreateEcalClusters",
                                                 towerTool = towersEcal,
                                                 nEtaWindow = 13, nPhiWindow = 13,
                                                 nEtaPosition = 5, nPhiPosition = 5,
                                                 nEtaDuplicates = 5, nPhiDuplicates = 5,
                                                 nEtaFinal = 13, nPhiFinal = 13,
                                                 energyThreshold = 10,
                                                 OutputLevel = DEBUG)
createEcalClusters.clusters.Path = "EcalClusters"


out = PodioOutput("out", filename = "~/FCCSW/condor/output_combCalo_reconstructionSW_"+str(particleType)+str(energy)+"GeV_bfield"+str(bfield)+"_part"+str(i)+".root",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *","drop ECalHits", "drop HCalHits"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
resegmentEcal.AuditExecute = True
createEcalClusters.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              resegmentEcal,
              createEcalClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = int(num_events),
    ExtSvc = [geoservice, podioevent, audsvc],
)
