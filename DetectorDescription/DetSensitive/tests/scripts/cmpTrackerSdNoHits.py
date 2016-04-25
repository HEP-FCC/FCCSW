from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
storeExt = EventStore(["out_simpleTrackerSD_2cm.root"])
for iev, event in enumerate(storeExt):
    genptcs = event.get('allGenParticles')
    assert(len(genptcs)==1.)
    hits = event.get('hits')
    hitsExt = len(hits)

storeInt = EventStore(["out_simpleTrackerSD_2cm.root"])
for iev, event in enumerate(storeInt):
    genptcs = event.get('allGenParticles')
    assert(len(genptcs)==1.)
    hits = event.get('hits')
    hitsInt = len(hits)

assert(hitsExt==hitsInt)
