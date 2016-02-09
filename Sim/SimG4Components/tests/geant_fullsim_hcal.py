from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

podioevent = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import G4SimSvc
geantservice = G4SimSvc("G4SimSvc", detector="G4DD4hepDetector", physicslist="G4FtfpBert", actions="G4FullSimActions")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detector='file:DetectorDescription/Detectors/compact/TestHCal.xml',
                    OutputLevel = DEBUG)

from Configurables import G4SimAlg, G4SaveCalHits
savehcaltool = G4SaveCalHits("G4SaveHCalHits", caloType = "HCal")
savehcaltool.DataOutputs.caloClusters.Path = "caloClusters"
savehcaltool.DataOutputs.caloHits.Path = "caloHits"
geantsim = G4SimAlg("G4SimAlg",
                        outputs = ["G4SaveCalHits/G4SaveHCalHits"] )
geantsim.DataInputs.genParticles.Path="allGenParticles"

from Configurables import PodioWrite, PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [podioevent, geoservice, geantservice], # order! geo needed by geant
                OutputLevel=DEBUG
 )
