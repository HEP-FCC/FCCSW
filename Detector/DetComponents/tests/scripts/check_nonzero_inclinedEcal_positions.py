from ROOT import gSystem
from EventStore import EventStore

if __name__ == "__main__":
    gSystem.Load("libdatamodelDict")
    store = EventStore(["positions_ecalInclinedSim.root"])

    for iev, event in enumerate(store):
        positions = event.get('Positions')
        for pos in positions:
            x = pos.position().x
            y = pos.position().y
            z = pos.position().z
            assert( not (x == 0 and y == 0 and z == 0))
