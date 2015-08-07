# example file for a user-defined momentum spectrum
# Michel De Cian, 15.11.2013


from Gauss.Configuration import *

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

#--Number of events
nEvts = 10000
LHCbApp().EvtMax = nEvts

##############################################

#--Pick beam conditions as set in AppConfig
from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Sim08-Beam4000GeV-md100-2012-nu2.5.py")

from Configurables import LHCbApp
#--Set database tags using those for Sim08
LHCbApp().DDDBtag = "Sim08-20130503-1"
LHCbApp().CondDBtag = "Sim08-20130503-1-vc-md100"

#############################################

from Gauss.Configuration import *
Gauss().Production = 'PGUN'
Gauss().DatasetName = "test"

Gauss().Phases = ["Generator","GenToMCTree"] 

#####################################################################

from Configurables import ParticleGun, FlatNParticles, MomentumSpectrum

pgun = ParticleGun()

pgun.ParticleGunTool = "MomentumSpectrum"
pgun.addTool( MomentumSpectrum , name = "MomentumSpectrum" )

pgun.MomentumSpectrum.PdgCodes = [ 511 , -511 ]
pgun.MomentumSpectrum.InputFile = "BSpectrum.root"
pgun.MomentumSpectrum.HistogramPath = "BSpectrum"
pgun.MomentumSpectrum.OutputLevel = 3

pgun.NumberOfParticlesTool = "FlatNParticles"
pgun.addTool( FlatNParticles , name = "FlatNParticles" )







