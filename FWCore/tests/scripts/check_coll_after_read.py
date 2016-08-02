from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
store = EventStore(["./pythia_test.root"])
store_after = EventStore(["./test.root"])

nevts = min(len(store_after), len(store))
for iev in range(nevts):
    event_after = store_after[iev]
    event = store[iev]

    particles_before = event.get("allGenParticles")
    particles_after = event_after.get('allGenParticles')
    assert(len(particles_before) == len(particles_after))

    for before, after in zip(particles_before, particles_after):
        assert(before.Core().P4.Px == after.Core().P4.Px)
        assert(before.Core().P4.Py == after.Core().P4.Py)
        assert(before.Core().P4.Pz == after.Core().P4.Pz)
