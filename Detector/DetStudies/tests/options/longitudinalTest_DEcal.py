from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhECalDigital/compact/FCChh_DECalBarrel_Mockup.xml'
],
                    OutputLevel = INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")
geantservice.G4commands += ["/run/setCut 0.1 mm"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# and a tool that saves the calorimeter hits
from Configurables import SimG4Alg, SimG4SaveCalHits, SimG4SingleParticleGeneratorTool
saveecaltool = SimG4SaveCalHits("saveECalHits",readoutNames = ["BarDECal_Readout"])
#saveecaltool.DataOutputs.caloClusters.Path = "ECalClusters"
saveecaltool.positionedCaloHits.Path = "positionedCaloHits"
saveecaltool.caloHits.Path = "ECalHits"

from Configurables import SimG4SingleParticleGeneratorTool
pgun=SimG4SingleParticleGeneratorTool("SimG4SingleParticleGeneratorTool",
                saveEdm=True,
                particleName="e-",
                energyMin=100000,energyMax=100000,
                etaMin=0,etaMax=0, 
#                phiMin=0, phiMax=0.001,
                vertexX=0,vertexY=0,vertexZ=0,
                OutputLevel =DEBUG)

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveCalHits/saveECalHits"],
                    eventProvider = pgun,
                    OutputLevel = DEBUG)

from Configurables import DECalLongitudinalTest
hist = DECalLongitudinalTest("hists",   
                                 readoutName = "BarDECal_Readout",
                                 layerFieldName = "layer",
                                 numLayers = 50, # one more because index starts at 1 - layer 0 will be always empty
                                 OutputLevel = DEBUG)
hist.deposits.Path="positionedCaloHits"

THistSvc().Output = ["rec DATAFILE='hist_test.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=False
THistSvc().OutputLevel=INFO


# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = "output_10GeV_1mmAir_0T_SiAirW.root"

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True
hist.AuditExecute = True

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out, hist],
                EvtSel = 'NONE',
                EvtMax = 10,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice, audsvc],
                OutputLevel = DEBUG
)
