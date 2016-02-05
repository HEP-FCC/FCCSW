from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioWrite, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")


from Configurables import DelphesSimulation
delphessimulation = DelphesSimulation("Simulation", filename="/afs/cern.ch/user/d/degrutto/public/forFCC/ZLEP_toAll.hepmc" , detectorcard = "Simulation/examples/delphes_card_CMS.tcl" , outputcollections =  [ "UniqueObjectFinder/electrons", "UniqueObjectFinder/muons",  "UniqueObjectFinder/jets",    "UniqueObjectFinder/photons",  "MissingET/momentum", "ScalarHT/energy" ] , debug_delphes= True )
#delphessimulation = DelphesSimulation("Simulation", filename="example_MyPythia.dat" , detectorcard = "Simulation/examples/delphes_card_FCC_basic.tcl" , outputcollections =  [ "ElectronEnergySmearing/electrons", "MuonMomentumSmearing/muons",  "JetEnergyScale/jets",  "MissingET/momentum", "ScalarHT/energy" ] )

# we shouls add b-jets and track inpact parameters, fixme
delphessimulation.DataOutputs.genparticles.Path = "genparticles"
delphessimulation.DataOutputs.particles.Path = "particles"
delphessimulation.DataOutputs.partons.Path = "partons"
delphessimulation.DataOutputs.muons.Path = "muons"
delphessimulation.DataOutputs.electrons.Path = "electrons"
delphessimulation.DataOutputs.photons.Path = "photons"
delphessimulation.DataOutputs.jets.Path = "jets"
delphessimulation.DataOutputs.mets.Path = "met"
delphessimulation.DataOutputs.hts.Path = "ht"


out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["drop *",
                      "keep genparticles",
                      "keep partons",
                      "keep particles",
                      "keep muons",
                      "keep electrons",
                      "keep photons",
                      "keep jets",
                      "keep met",
                      "keep ht"

]

ApplicationMgr(
    TopAlg = [
              delphessimulation,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = 10000, # n of events are decided by Delphes, fixme!
    ExtSvc = [podioevent],
    #                EventLoop = eventloopmgr,
    #                OutputLevel=DEBUG
 )

