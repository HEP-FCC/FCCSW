

### \file
### \ingroup BasicExamples
### | **input (alg)**               | other algorithms                 | **output (alg)**                              |
### |-------------------------------|----------------------------------|-----------------------------------------------|
### | read event data from an edm file | overlay event data with pileup from file(s)  | write merged EDM output to ROOT file using PODIO |

from Gaudi.Configuration import *


# list of names of files with pileup event data -- to be overlaid
pileupFilenames = ["tracker_with_field.root"]
# the file containing the signal events
signalFilename = "output_geant_pgun_fullsim.root"
# the collections to be read from the signal event
signalCollections = ["allGenVertices", "allGenParticles", "hits", "positionedHits"]


# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc", input=signalFilename)

# use PodioInput for Signal
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=signalCollections, OutputLevel=DEBUG)

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
pileuptool = ConstPileUp("MyPileupTool", numPileUpEvents=1)

# algorithm for the overlay
from Configurables import PileupOverlayAlg
overlay = PileupOverlayAlg()
overlay.pileupFilenames = pileupFilenames
overlay.randomizePileup = False
overlay.mergeTools = ["PileupParticlesMergeTool/MyPileupParticlesMergeTool",
  "PileupTrackHitMergeTool/MyTrackHitMergeTool"]
overlay.PileUpTool = pileuptool






# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = "output_overlaid_with_pileup.root"

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[podioinput, overlay, out],
                EvtSel='NONE',
                EvtMax=1,
                ExtSvc=[podioevent,],
                OutputLevel=DEBUG
 )
