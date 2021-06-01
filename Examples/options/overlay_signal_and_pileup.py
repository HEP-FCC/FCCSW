### | read event data from an edm file | overlay event data with pileup from file(s)  | write merged EDM output to ROOT file using PODIO |

from Gaudi.Configuration import *

from Configurables import ApplicationMgr
ApplicationMgr().EvtSel = 'NONE'
ApplicationMgr().EvtMax = 1

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")
# the file containing the signal events
podioevent.input = "output_geant_pgun_fullsim.root"
ApplicationMgr().ExtSvc += [podioevent]

# use PodioInput for Signal
from Configurables import PodioInput
podioinput = PodioInput("PodioReader")
# the collections to be read from the signal event
podioinput.collections = [
                "allGenVertices", 
                "allGenParticles", 
                "hits", 
                "positionedHits",
              ]
ApplicationMgr().TopAlg += [podioinput]

# configure the tools for the data overlay
# edm data from generation: particles and vertices
from Configurables import PileupParticlesMergeTool
particlemergetool = PileupParticlesMergeTool("MyPileupParticlesMergeTool")
# branchnames for the pileup
particlemergetool.genParticlesBranch = "allGenParticles"
particlemergetool.genVerticesBranch = "allGenVertices"
# branchnames for the signal
particlemergetool.signalGenVertices.Path = "allGenVertices"
particlemergetool.signalGenParticles.Path = "allGenParticles"
# branchnames for the output
particlemergetool.mergedGenParticles.Path = "overlaidGenParticles"
particlemergetool.mergedGenVertices.Path = "overlaidGenVertices"

# edm data from simulation: hits and positioned hits
from Configurables import PileupTrackHitMergeTool
trackhitsmergetool = PileupTrackHitMergeTool("MyTrackHitMergeTool")
# branchnames for the pileup
trackhitsmergetool.pileupHitsBranch = "hits"
trackhitsmergetool.pileupPositionedHitsBranch = "positionedHits"
# branchnames for the signal
trackhitsmergetool.signalHits = "hits"
trackhitsmergetool.signalPositionedHits = "positionedHits"
# branchnames for the output
trackhitsmergetool.mergedHits = "overlaidTrackHits"
trackhitsmergetool.mergedPositionedHits = "overlaidPositionedTrackHits"

# use the pileuptool to specify the number of pileup
from Configurables import ConstPileUp
pileuptool = ConstPileUp("MyPileupTool")
pileuptool.numPileUpEvents = 1

# algorithm for the overlay
from Configurables import PileupOverlayAlg
overlay = PileupOverlayAlg()
# list of names of files with pileup event data -- to be overlaid
overlay.pileupFilenames = ["tracker_with_field.root"]
overlay.randomizePileup = False
overlay.mergeTools = [
                      "PileupParticlesMergeTool/MyPileupParticlesMergeTool",
                      "PileupTrackHitMergeTool/MyTrackHitMergeTool",
                     ]
overlay.PileUpTool = pileuptool
ApplicationMgr().TopAlg += [overlay]

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out")
out.OutputLevel = DEBUG
out.outputCommands = ["keep *"]
out.filename = "output_overlaid_with_pileup.root"
ApplicationMgr().TopAlg += [out]

