from ROOT import gSystem
from EventStore import EventStore
from numpy import testing

centre_x = [1977.25, -726.366, 2130.49, -2045.03, -2236.08]
centre_y = [9.07315, 2114.81, 679.002, 652.057, -1.38118]
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
