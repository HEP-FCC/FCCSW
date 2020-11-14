from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libedm4hepDict")
store = EventStore(["./out_fast_simple.root"])

for iev, event in enumerate(store):
    particles = event.get('RecParticlesSmeared')
    print(len(particles))
    assert(len(particles) > 0)
