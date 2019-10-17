from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
store = EventStore(["./out_fast_simple.root"])

for iev, event in enumerate(store):
    particles = event.get('smearedParticles')
    print(len(particles))
    assert(len(particles) > 0)
