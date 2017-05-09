from Gaudi.Configuration import *

from Configurables import HepMCFileReader, GenAlg

readertool = HepMCFileReader("Reader", Filename="/eos/project/f/fccsw-web/testsamples/testHepMCrandom.dat")
reader = GenAlg("Reader", SignalProvider=readertool)
reader.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:../data/TestPrimitives.xml'])

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist='SimG4TestPhysicsList', actions='SimG4FullSimActions')

from Configurables import SimG4Alg, SimG4SaveCalHits
geantsim = SimG4Alg("SimG4Alg")


# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [geoservice, geantservice],
                OutputLevel=INFO
 )
