from ROOT import gSystem
from EventStore import EventStore
from numpy import testing

centre_x = [1968.99, -675.552, 2142.64, -2023.78, -2229.37]
centre_y = [-0.8434, 2081.93, 685.885, 658.538, 6.85499]
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
