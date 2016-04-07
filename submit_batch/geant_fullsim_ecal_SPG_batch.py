#JANA: variables ENE (energy in GeV), BFIELD (0,1), EVTMAX (number of events) to be defined before running

from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# Magnetic field
from Configurables import G4ConstantMagneticFieldTool
if BFIELD==1:
    field = G4ConstantMagneticFieldTool("G4ConstantMagneticFieldTool",FieldOn=True)
else: 
    field = G4ConstantMagneticFieldTool("G4ConstantMagneticFieldTool",FieldOn=False)
 

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:DetectorDescription/Detectors/compact/FCChh_DectMaster.xml',
                                         'file:DetectorDescription/Detectors/compact/FCChh_ECalBarrel_Mockup.xml'
                                        ],
                    OutputLevel = DEBUG)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import G4SimSvc, G4SingleParticleGeneratorTool 
# Configures the Geant simulation: geometry, physics list and user actions
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist="G4FtfpBert",
particleGenerator=G4SingleParticleGeneratorTool("G4SingleParticleGeneratorTool",
                ParticleName="e-",eMin=ENE,eMax=ENE),
                actions="G4FullSimActions") 


# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import G4SimAlg, G4SaveCalHits
# and a tool that saves the calorimeter hits with a name "G4SaveCalHits/saveHCalHits"
savehcaltool = G4SaveCalHits("saveHCalHits", caloType = "HCal")
savehcaltool.DataOutputs.caloClusters.Path = "HCalClusters"
savehcaltool.DataOutputs.caloHits.Path = "HCalHits"

saveecaltool = G4SaveCalHits("saveECalHits", caloType = "ECal")
saveecaltool.DataOutputs.caloClusters.Path = "ECalClusters"
saveecaltool.DataOutputs.caloHits.Path = "ECalHits"

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
geantsim = G4SimAlg("G4SimAlg",
                        outputs= ["G4SaveCalHits/saveHCalHits","G4SaveCalHits/saveECalHits"])

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = EVTMAX,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
)
