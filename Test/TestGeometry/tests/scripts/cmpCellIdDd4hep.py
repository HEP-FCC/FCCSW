from ROOT import gSystem
from EventStore import EventStore
from array import array
from math import sqrt, pow

cellNo = 11
cellSize = 5
bitfieldsize = 4

def retrieve(sign, no, mask, offset):
    id = (no & mask)
    id = id >> offset
    if(sign):
        id = id - 2**bitfieldsize if id >= 2**(bitfieldsize-1) else id
    return id

def z(cellId, sign):
    return retrieve(sign, cellId, 0b000000001111,0)

def y(cellId, sign):
    return retrieve(sign, cellId, 0b000011110000,1*bitfieldsize)

def x(cellId, sign):
    return retrieve(sign, cellId, 0b111100000000,2*bitfieldsize)

def cellPosDd4hep(cellId, sign):
    if(sign):
        return array('d',(x(cellId, sign)*cellSize,y(cellId, sign)*cellSize,z(cellId, sign)*cellSize))
    return array('d',((x(cellId, sign)-cellNo/2)*cellSize,
                      (y(cellId, sign)-cellNo/2)*cellSize,
                      (z(cellId, sign)-cellNo/2)*cellSize))


if __name__ == "__main__":
    gSystem.Load("libdatamodelDict")
    storeSeg = EventStore(["testCellId_dd4hep_segmentation.root"])
    storeVol = EventStore(["testCellId_dd4hep_volumes.root"])

    segPos = []
    for iev, event in enumerate(storeSeg):
        hits = event.get('caloHits')
        for hit in hits:
            cellId = hit.cellId()
            segPos.append(cellPosDd4hep(cellId,True))
    volPos = []
    for iev, event in enumerate(storeVol):
        hits = event.get('caloHits')
        for hit in hits:
            cellId = hit.cellId()
            volPos.append(cellPosDd4hep(cellId,False))
    assert(segPos == volPos)
