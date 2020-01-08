from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCDumper, HepMCJetClustering
from Configurables import HepMCHistograms, JetHistograms


#TODO: best to run pythia here?
from Configurables import MomentumRangeParticleGun
from GaudiKernel import PhysicalConstants as constants
guntool = MomentumRangeParticleGun()
guntool.ThetaMin = 0 
guntool.ThetaMax = 2 * constants.pi 
guntool.PdgCodes = [11]
from Configurables import GenAlg
gen = GenAlg()
gen.SignalProvider=guntool
gen.hepmc.Path = "hepmc"


# creates histograms for HepMC
genHisto = HepMCHistograms("GenHistograms")
genHisto.hepmc.Path="hepmc"

# runs sample jet clustering algorithm
jets = HepMCJetClustering("ktCluster")
jets.JetAlgorithm = "kt"
jets.RecominbationScheme = "E"
jets.ConeRadius = 0.7
jets.PtMin = 1
jets.hepmc.Path="hepmc"
jets.jets.Path="ktJets"

# creates histograms for jets
jetHisto = JetHistograms("JetHistograms")
jetHisto.jets.Path="ktJets"

# save all histograms
THistSvc().Output = ["rec DATAFILE='GenHistograms.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=VERBOSE

# ApplicationMgr
ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[gen,genHisto,jets,jetHisto])
