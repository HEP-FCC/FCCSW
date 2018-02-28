from ROOT import gSystem
from EventStore import EventStore
from numpy import testing

signed = False

def retrieve(no, mask, offset):
    id = (no & mask)
    id = id >> offset
    if(signed):
        id = id - 2**bitfieldsize if id >= 2**(bitfieldsize-1) else id
    return id

def sublayer(cellId):
    # for bitfield system:4,subsystem:1,type:3,subtype:3,layer:8,sublayer:8,eta:10,phi:10
    return retrieve(cellId, 0b00000000000000000000111111110000000000000000000, 19)

if __name__ == "__main__":
    gSystem.Load("libdatamodelDict")
    store = EventStore(["positions_ecalEndcapSim.root"])

    for iev, event in enumerate(store):
        positions = event.get('caloPositions')
        for pos in positions:
            x = pos.position().x
            y = pos.position().y
            z = pos.position().z
            cellId = pos.core().cellId
            layerId = sublayer(cellId)
            # - 1 because the sublayer ID starts form ID=1
            layerId -= 1
            # detector begins at 5300, cryostat thickness of 50, bath of 90, margin 1.65, readout 1.2  => 5442.85
            if layerId == 0:
                # first active half-disc
                layer_z = 5443.1
            else:
                # other discs (middle in the middle of readout), shifted by = 1.5+1.2+1 = 3.7
                layer_z = 5445.95 + (layerId - 1) * 3.7
            testing.assert_allclose(x , 0, 1e-3, 1e-3)
            testing.assert_allclose(y , 0, 1e-3, 1e-3)
            testing.assert_allclose(z , layer_z, 1e-3, 1e-3)
