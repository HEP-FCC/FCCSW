from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, FCCDataSvc

albersevent   = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.Outputs.hepmc.Path = "hepmc"

from Configurables import HepMCToGeantConverter
hepmc2geant = HepMCToGeantConverter("ConverterHepMCToGeant")
hepmc2geant.Inputs.hepmc.Path="hepmc"
hepmc2geant.Outputs.g4event.Path = "g4event"

from Configurables import Geant4Simulation
geant4simulation = Geant4Simulation("Geant4Simulation")
geant4simulation.Inputs.g4event.Path = "g4event"
# geant4simulation.Outputs.particles.Path = "particles"

from Configurables import AlbersWrite, AlbersOutput
out = AlbersOutput("out")
out.outputCommands = ["drop *"]

ApplicationMgr( TopAlg = [reader, hepmc2geant, geant4simulation, out],
                EvtSel = 'NONE',
                EvtMax   = 20,
                ExtSvc = [albersevent],
#                EventLoop = eventloopmgr,
OutputLevel=DEBUG
 )
