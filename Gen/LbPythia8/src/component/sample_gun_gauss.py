from Gauss.Configuration import *

# EXAMPLE: 
# gaudirun.py $GAUSSOPTS/Gauss-2012.py $GAUSSOPTS/GenStandAlone.py ./sample_gun_gauss.py

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1

#--Number of events
nEvts = 10
LHCbApp().EvtMax = nEvts
# LHCbApp().OutputLevel = DEBUG

#==========================================================

from Gauss.Configuration import *
from Configurables import Generation
from Configurables import MinimumBias, Special
from Configurables import Pythia8Gun


from Configurables import FixedNInteractions
Generation().PileUpTool = 'FixedNInteractions'
Generation().addTool( FixedNInteractions )
Generation().FixedNInteractions.NInteractions = 1

Generation().SampleGenerationTool = 'MinimumBias'
Generation().addTool( MinimumBias )
Generation().MinimumBias.ProductionTool = "Pythia8Gun"
Generation().MinimumBias.addTool( Pythia8Gun )


#==========================================================

from GaudiKernel.SystemOfUnits import GeV

# List of particle to be used in the gun. PDG-code.
Generation().MinimumBias.Pythia8Gun.PDGList = [ 23 ] # Z-boson

# Energy range of the beam. 
Generation().MinimumBias.Pythia8Gun.EnergyMin = 1000*GeV
Generation().MinimumBias.Pythia8Gun.EnergyMax = 8000*GeV


# You can use Pythia8 syntax to control the decay channel.
# For example, Z -> mu+ mu-
Generation().MinimumBias.Pythia8Gun.Commands += [
  "23:onMode = off",
  "23:onIfMatch = 13 -13"
]

