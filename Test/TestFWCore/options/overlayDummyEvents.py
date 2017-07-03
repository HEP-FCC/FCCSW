
from Gaudi.Configuration import *


pileupFilenames = ["dummyEventData2.root", "dummyEventData1.root", "dummyEventData3.root"]
#pileupFilenames = ["dummyEventData3.root"]

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

#from Configurables import CreateExampleEventData
#podioinput = PodioInput("PodioReader", collections=signalCollections, OutputLevel=DEBUG)

from Configurables import PileupParticlesMergeTool

particlemergetool = PileupParticlesMergeTool("MyPileupParticlesMergeTool")
particlemergetool.genParticlesBranch = "genParticles"
particlemergetool.genVerticesBranch = "genVertices"
particlemergetool.signalGenParticles.Path = "genParticles"
particlemergetool.signalGenVertices.Path = "genVertices"
particlemergetool.mergedGenParticles.Path = "overlaidGenParticles"
particlemergetool.mergedGenVertices.Path = "overlaidGenVertices"

from Configurables import PileupTrackHitsMergeTool
trackhitsmergetool = PileupTrackHitsMergeTool("MyTrackHitsMergeTool")
trackhitsmergetool.pileupTrackHitsBranch = "trackHits"
trackhitsmergetool.pileupPositionedTrackHitsBranch = "positionedTrackHits"
trackhitsmergetool.signalTrackHits = "trackHits"
trackhitsmergetool.signalPositionedTrackHits = "positionedTrackHits"
trackhitsmergetool.mergedTrackHits = "overlaidTrackHits"
trackhitsmergetool.mergedPositionedTrackHits = "overlaidPositionedTrackHits"

from Configurables import ConstPileUp

pileuptool = ConstPileUp("MyPileupTool", numPileUpEvents=12)

from Configurables import PileupOverlayAlg

overlay = PileupOverlayAlg()
overlay.pileupFilenames = pileupFilenames
overlay.randomizePileup = False
overlay.mergeTools = ["PileupParticlesMergeTool/MyPileupParticlesMergeTool",
  "PileupTrackHitsMergeTool/MyTrackHitsMergeTool"]
overlay.PileUpTool = pileuptool


from Configurables import CreateExampleEventData
producer = CreateExampleEventData()
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
out.outputCommands = ["keep *"]
out.filename = "dummyEventDataOverlaid.root"

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [producer, overlay,
                   out],
                EvtSel = 'NONE',
                EvtMax   = 300,
                ExtSvc = [podioevent,],
                OutputLevel=DEBUG
 )
