from ROOT import gSystem
from EventStore import EventStore
from numpy import testing

centre_x = [1970.55, -676.869, 2139.19, -2029.88, -2238.6]
centre_y = [1.54627, 2086.33, 693.37, 659.306, -3.59223]
centre_z = [0, 0, 0, 0, 0]

if __name__ == "__main__":
    gSystem.Load("libdatamodelDict")
    store = EventStore(["positions_ecalInclinedSim.root"])

    for iev, event in enumerate(store):
        positions = event.get('Positions')
        for pos in positions:
            x = pos.position().x
            y = pos.position().y
            z = pos.position().z
            testing.assert_allclose(x , centre_x[iev], 1e-3, 1e-3)
            testing.assert_allclose(y , centre_y[iev], 1e-3, 1e-3)
            testing.assert_allclose(z , centre_z[iev], 1e-3, 1e-3)
