from ROOT import gSystem
from EventStore import EventStore


# refer to HepMCFromScratch for the particle tree that is checked here

if __name__ == "__main__":
    gSystem.Load("libdatamodelDict")
    store = EventStore(["test_hepmc_graph.root"])
    for iev, event in enumerate(store):
        parts = event.get('allGenParticles')
        vertices = event.get('allGenVertices')
        # event contains 8 particles, intermediate and final
        assert(len(parts) == 8)
        part1 = parts[3]
        part2 = parts[4]
        endvertex1 = part1.endVertex()
        startvertex2 = part2.startVertex()
        # part2 is the daughter of part1, so they should share a vertex
        assert (endvertex1 == startvertex2)

