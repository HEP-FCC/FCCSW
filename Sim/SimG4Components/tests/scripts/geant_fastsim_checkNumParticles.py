from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
store = EventStore(["./out_fast_simple.root"])

entries = [82, 190, 276, 29, 24, 62, 115, 69, 2, 262]

for iev, event in enumerate(store):
    if(iev<10):
        particles = event.get('smearedParticles')
        print('test=',entries[iev],len(particles))
        assert(len(particles) == entries[iev])
