from Gaudi.Configuration import *

# data service
from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")

from Configurables import MomentumRangeParticleGun
from GaudiKernel import PhysicalConstants as constants
guntool = MomentumRangeParticleGun()
guntool.ThetaMin = 0 
guntool.ThetaMax = 2 * constants.pi 
guntool.PdgCodes = [11]
from Configurables import GenAlg
gen = GenAlg()
gen.SignalProvider=guntool
# In the following line,
#   reader.YYY.Path = "XXX"
# YYY matches the string passed to declareOutput in the constructor of the algorithm (here "hepmc")
# XXX declares a name for the product (here the product is HepMC::GenEvent: "hepmcevent")
gen.hepmc.Path = "hepmc"


from Configurables import HepMCDumper
## dumps the HepMC::GenEvent
dumper = HepMCDumper("Dumper")
# the input product name matches the output product name of the previous module ("hepmcevent")
# the string "hepmc" is passed in the constructor of HepMCDumper in DeclareProperty method
#     note that it is only by coincidence the same as in HepMCReader
dumper.hepmc.Path="hepmc"

from Configurables import CreateSampleJet, PodioOutput
## dummy algorithm creating a sample jet
podiowrite = CreateSampleJet("SampleJet",OutputLevel=DEBUG)
podiowrite.podioJets.Path = "podioJets"

from Configurables import PodioOutput
## PODIO algorithm
out = PodioOutput()
out.filename = "out_example_options.root"

from Configurables import ApplicationMgr
##
ApplicationMgr(
    # all algorithms should be put here
    TopAlg=[gen, dumper, podiowrite, out],
    EvtSel='NONE',
    # number of events
    EvtMax=1,
    # all services should be put here
    ExtSvc=[podioevent],
    # possible: DEBUG, INFO, WARNING, ERROR, FATAL
    OutputLevel=INFO
)
