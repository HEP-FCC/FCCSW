from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
store = EventStore(["./out_fast_simple.root"])

entries = [110, 258, 387, 41, 41, 85, 152, 98, 10, 345]

for iev, event in enumerate(store):
    if(iev<10):
        particles = event.get('smearedParticles')
        print('test=',entries[iev],len(particles))
        assert(len(particles) == entries[iev])
