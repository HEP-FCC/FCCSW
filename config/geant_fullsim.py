from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

albersevent = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.Outputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.Inputs.hepmc.Path="hepmc"
hepmc_converter.Outputs.genparticles.Path="all_genparticles"
hepmc_converter.Outputs.genvertices.Path="all_genvertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", OutputLevel = VERBOSE)

from Configurables import Geant4Simulation
geant4simulation = Geant4Simulation("Geant4Simulation", simtype="full")
geant4simulation.Inputs.genvertices.Path="all_genvertices"
geant4simulation.Inputs.genparticles.Path="all_genparticles"
geant4simulation.Outputs.particles.Path = "recparticles"
geant4simulation.Outputs.particleassociation.Path = "particleMCparticle"

from Configurables import Analyse
analysis = Analyse("Analyse")
analysis.Inputs.particleassociation.Path = "particleMCparticle"

from Configurables import AlbersWrite, AlbersOutput
out = AlbersOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg = [reader, hepmc_converter, geant4simulation, analysis, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [albersevent, geoservice],
                OutputLevel=DEBUG
 )
