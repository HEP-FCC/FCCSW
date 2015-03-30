from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

albersevent   = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.Outputs.hepmc.Path = "hepmc"

from Configurables import Geant4Simulation
geant4simulation = Geant4Simulation("Geant4Simulation")
geant4simulation.Inputs.hepmcevent.Path = "hepmc"
# geant4simulation.Outputs.particles.Path = "particles"

from Configurables import AlbersWrite, AlbersOutput
out = AlbersOutput("out")
out.outputCommands = ["drop *"]

ApplicationMgr( TopAlg = [reader, geant4simulation, out],
                EvtSel = 'NONE',
                EvtMax   = 10,
                ExtSvc = [albersevent],
                OutputLevel=DEBUG
 )
