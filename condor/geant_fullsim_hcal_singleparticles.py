import os
import numpy as np

#loads array of random seeds from file
seed_array = np.loadtxt('/afs/cern.ch/user/c/cneubuse/FCCSW/condor/seeds.txt',dtype='int',delimiter=',')

#set these in the .sh script
energy=100e3
num_events=10
magnetic_field=1
i=5
cluster=1

from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          #'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'
                                        ],
                    OutputLevel = INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")


# Magnetic field
from Configurables import SimG4ConstantMagneticFieldTool
if magnetic_field==1:
    field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",FieldOn=True,IntegratorStepper="ClassicalRK4")
else:
    field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",FieldOn=False)

#Setting random seeds for Geant4 simulations
geantservice.G4commands += ["/random/setSeeds "+str(seed_array[i-1])+" 0"]
#since the loop to generate the subjobs begins with 1, we need (i-1) to index

#range cut
geantservice.G4commands += ["/run/setCut 0.1 mm"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# and a tool that saves the calorimeter hits
from Configurables import SimG4Alg, SimG4SaveCalHits, InspectHitsCollectionsTool
savehcaltool = SimG4SaveCalHits("saveHCalHits",readoutNames = ["BarHCal_Readout"])
savehcaltool.DataOutputs.positionedCaloHits.Path = "HCalPositionedHits"
savehcaltool.DataOutputs.caloHits.Path = "HCalHits"
#change INFO to DEBUG for printout of each deposit
inspect =  InspectHitsCollectionsTool("inspect", readoutNames = ["BarHCal_Readout"], OutputLevel = INFO)

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
from Configurables import SimG4SingleParticleGeneratorTool
pgun=SimG4SingleParticleGeneratorTool("SimG4SingleParticleGeneratorTool",saveEdm=True,
                particleName="pi-",energyMin=energy,energyMax=energy,etaMin=0.36,etaMax=0.36,
                OutputLevel =DEBUG)

geantsim = SimG4Alg("SimG4Alg",
                       outputs= ["SimG4SaveCalHits/saveHCalHits", "InspectHitsCollectionsTool/inspect"],
                       eventProvider=pgun,
                       OutputLevel=DEBUG)

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = "output_pi"+str(int(energy/1e3))+"_part"+str(i)+".v1.root"

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = num_events,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice, audsvc],
                OutputLevel=DEBUG
)
