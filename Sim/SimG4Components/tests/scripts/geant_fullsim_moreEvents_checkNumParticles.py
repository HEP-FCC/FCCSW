from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
store = EventStore(['out_full_moreEvents.root'])

entries = [300, 622, 942, 152, 135, 233, 501, 286, 77, 820]

for iev, event in enumerate(store):
    if(iev<10):
        particles = event.get('allGenParticles')
        print(entries[iev],len(particles))
        assert(len(particles) == entries[iev])
