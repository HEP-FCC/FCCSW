from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
store = EventStore(['test_geant_fullsim_moreEvents.root'])

#entries = [300, 622, 942, 152, 135, 233, 501, 286, 77, 820]
entries = [1] * 10


for iev, event in enumerate(store):
    if(iev<10):
        particles = event.get('allGenParticles')
        print(entries[iev],len(particles))
        assert(len(particles) == entries[iev])
