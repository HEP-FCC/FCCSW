from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

albersevent = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="all_genparticles"
hepmc_converter.DataOutputs.genvertices.Path="all_genvertices"

from Configurables import sim__GeantSvc
geantservice = sim__GeantSvc("GeantSvc", config="GeantFullSimConfig")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detector='file:DetectorDescription/Detectors/compact/TestTracker.xml',
                   # sensitive='file:DetectorDescription/Detectors/compact/TestTracker_geant4.xml',
                    OutputLevel = DEBUG)

from Configurables import sim__GeantFullSimAlg
geantsim = sim__GeantFullSimAlg("GeantFullSimAlg")
geantsim.DataInputs.genParticles.Path="all_genparticles"
geantsim.DataOutputs.trackClusters.Path = "clusters"


from Configurables import AlbersWrite, AlbersOutput
out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
