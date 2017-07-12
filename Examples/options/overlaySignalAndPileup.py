
from Gaudi.Configuration import *


pileupFilenames = ["tracker_with_field.root"]
signalFilename = "output_geant_pgun_fullsim.root"
signalCollections = ["allGenVertices", "allGenParticles", "hits", "positionedHits"]


# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc", input=signalFilename)

from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=signalCollections, OutputLevel=DEBUG)

from Configurables import PileupParticlesMergeTool

particlemergetool = PileupParticlesMergeTool("MyPileupParticlesMergeTool")
particlemergetool.genParticlesBranch = "allGenParticles"
particlemergetool.genVerticesBranch = "allGenVertices"
particlemergetool.signalGenVertices.Path = "allGenVertices"
particlemergetool.signalGenParticles.Path = "allGenParticles"
particlemergetool.mergedGenParticles.Path = "overlaidGenParticles"
particlemergetool.mergedGenVertices.Path = "overlaidGenVertices"

from Configurables import PileupTrackHitMergeTool
trackhitsmergetool = PileupTrackHitMergeTool("MyTrackHitMergeTool")
trackhitsmergetool.pileupHitsBranch = "hits"
trackhitsmergetool.pileupPositionedHitsBranch = "positionedHits"
trackhitsmergetool.signalHits = "hits"
trackhitsmergetool.signalPositionedHits = "positionedHits"
trackhitsmergetool.mergedHits = "overlaidTrackHits"
trackhitsmergetool.mergedPositionedHits = "overlaidPositionedTrackHits"

from Configurables import ConstPileUp

pileuptool = ConstPileUp("MyPileupTool", numPileUpEvents=1)

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
