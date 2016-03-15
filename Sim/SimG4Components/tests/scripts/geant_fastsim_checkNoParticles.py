from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
store = EventStore(["./out_fast_simple.root"])

entries = [155, 126, 72, 66, 46, 46, 67, 52, 186, 41]

for iev, event in enumerate(store):
    if(iev<10):
        particles = event.get('smearedParticles')
        print(entries[iev])
        print(len(particles))
        assert(len(particles) == entries[iev])
