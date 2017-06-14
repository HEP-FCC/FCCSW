from ROOT import gSystem
from EventStore import EventStore
from numpy import testing

centre_x = [1970.66, -678.08, 2140.49, -2028.41, -2237.05]
centre_y = [0.0856215, 2084.55, 683.874, 657.823, 3.22058]
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
