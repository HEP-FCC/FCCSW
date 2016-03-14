from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
store = EventStore(['out_full.root'])
for iev, event in enumerate(store):
    particles = event1.get('allGenParticles')
    assert(len(particles) == 411)
