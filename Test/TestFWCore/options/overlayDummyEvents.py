from Gaudi.Configuration import *

pileupFilenames = ["dummyEventData2.root", "dummyEventData1.root", "dummyEventData3.root",]

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import PileupParticlesMergeTool
particlemergetool = PileupParticlesMergeTool("MyPileupParticlesMergeTool")
particlemergetool.genParticlesBranch = "genParticles"
particlemergetool.genVerticesBranch = "genVertices"
particlemergetool.signalGenParticles.Path = "genParticles"
particlemergetool.signalGenVertices.Path = "genVertices"
particlemergetool.mergedGenParticles.Path = "overlaidGenParticles"
particlemergetool.mergedGenVertices.Path = "overlaidGenVertices"

from Configurables import PileupTrackHitMergeTool
trackhitsmergetool = PileupTrackHitMergeTool("MyTrackHitsMergeTool")
trackhitsmergetool.pileupHitsBranch = "trackHits"
trackhitsmergetool.pileupPositionedHitsBranch = "positionedTrackHits"
trackhitsmergetool.signalHits = "trackHits"
trackhitsmergetool.signalPositionedHits = "positionedTrackHits"
trackhitsmergetool.mergedHits = "overlaidTrackHits"
trackhitsmergetool.mergedPositionedHits = "overlaidPositionedTrackHits"

from Configurables import ConstPileUp
pileuptool = ConstPileUp("MyPileupTool", numPileUpEvents=12)

from Configurables import PileupOverlayAlg
overlay = PileupOverlayAlg()
overlay.pileupFilenames = pileupFilenames
overlay.randomizePileup = False
overlay.mergeTools = ["PileupParticlesMergeTool/MyPileupParticlesMergeTool",
  "PileupTrackHitMergeTool/MyTrackHitsMergeTool"]
overlay.PileUpTool = pileuptool

from Configurables import CreateExampleEventData
producer = CreateExampleEventData()
producer.magicNumberOffset = 300
producer.trackhits.Path = "trackHits"
producer.positionedtrackhits.Path = "positionedTrackHits"
producer.calohits.Path = "caloHits"
producer.positionedcalohits.Path = "positionedCaloHits"
producer.genparticles.Path = "genParticles"
producer.genvertices.Path = "genVertices"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["drop *", "keep overlaid*"]
out.filename = "dummyEventDataOverlaid.root"

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[producer, overlay, out],
                EvtSel='NONE',
                EvtMax=300,
                ExtSvc=[podioevent,],
                OutputLevel=DEBUG,
  )
