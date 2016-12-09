from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper, HepMCJetClustering
from Configurables import HepMCHistograms, JetHistograms

# reads HepMC text file and write the HepMC::GenEvent to the data service
reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.6/testsamples/example_MyPythia.dat")
reader.hepmc.Path = "hepmc"

# dumps the HepMC::GenEvent
dumper = HepMCDumper("Dumper")
dumper.hepmc.Path="hepmc"

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
               TopAlg=[reader,dumper,genHisto,jets,jetHisto])
