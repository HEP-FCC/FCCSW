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
        assert(before.core().p4.px == after.core().p4.px)
        assert(before.core().p4.py == after.core().p4.py)
        assert(before.core().p4.pz == after.core().p4.pz)
