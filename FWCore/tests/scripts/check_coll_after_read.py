from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
#store = EventStore(["./pythia_test.root"])
store_after = EventStore(["./test.root"])
#print store
print(store_after)


for event in store_after:

    particles_before = event.get("allGenParticles")
    print(len(particles_before))


