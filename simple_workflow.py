from Gaudi.Configuration import *

from Configurables import ApplicationMgr, HepMCReader, HepMCConverter, JetClustering, FCCDataSvc, AlbersWrite, AlbersOutput

albersevent   = FCCDataSvc("EventDataSvc")

reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
reader.Outputs.hepmc.Path = "hepmc"

hepmc_converter = HepMCConverter("Converter")
hepmc_converter.Inputs.hepmc.Path="hepmc"
hepmc_converter.Outputs.genparticles.Path="genparticles"

jet_clustering = JetClustering("JetClustering")
jet_clustering.Inputs.genparticles.Path='genparticles'
jet_clustering.Outputs.jets.Path='genjets'

out = AlbersOutput("out",
                   OutputLevel=DEBUG)

ApplicationMgr( TopAlg = [reader,hepmc_converter,jet_clustering, out],
                EvtSel = 'NONE',
                EvtMax   = 2,
                ExtSvc = [albersevent],
                #                EventLoop = eventloopmgr,
#                OutputLevel=DEBUG
 )

