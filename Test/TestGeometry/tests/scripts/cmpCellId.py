from ROOT import gSystem
from EventStore import EventStore
from array import array
from math import sqrt, pow

cellNo = 11
cellSize = 5
bitfieldsize = 4
signed = True

def retrieve(no, mask, offset):
    id = (no & mask)
    id = id >> offset
    if(signed):
        id = id - 2**bitfieldsize if id >= 2**(bitfieldsize-1) else id
    return id

def z(cellId):
    return retrieve(cellId, 0b000000001111,0)

def y(cellId):
    return retrieve(cellId, 0b000011110000,1*bitfieldsize)

def x(cellId):
    return retrieve(cellId, 0b111100000000,2*bitfieldsize)

def cellIdDd4hep(cellId):
    return array('d',(x(cellId),y(cellId),z(cellId)))

def cellPosDd4hep(cellId):
    return array('d',(x(cellId)*cellSize,y(cellId)*cellSize,z(cellId)*cellSize))

def cellIdGdml(cellId):
    z = cellId%cellNo
    x = cellId/(cellNo*cellNo)
    y = cellId/cellNo%cellNo
    return array('d',(x,y,z))

def cellPosGdml(cellId):
    z = (cellId%cellNo-cellNo/2)*cellSize
    x = ((cellId/(cellNo*cellNo))-cellNo/2)*cellSize
    y = ((cellId/cellNo%cellNo)-cellNo/2)*cellSize
    return array('d',(x,y,z))

def cmpId(cellIdGdml, cellIdDd4hep):
    IdGdml = cellIdGdml(cellIdGdml)
    IdDd4hep = cellIdDd4hep(cellIdDd4hep)
    return bool(IdGdml == IdDd4hep)

if __name__ == "__main__":
    gSystem.Load("libdatamodelDict")
    storeGdml = EventStore(["testCellId_gdml.root"])
    storeDD4hep = EventStore(["testCellId_dd4hep.root"])

    gdmlPos = []
    for iev, event in enumerate(storeGdml):
        hits = event.get('caloHits')
        for hit in hits:
            cellId = hit.cellId()
            gdmlPos.append(cellPosGdml(cellId))
    dd4hepPos = []
    for iev, event in enumerate(storeDD4hep):
        hits = event.get('caloHits')
        for hit in hits:
            cellId = hit.cellId()
            dd4hepPos.append(cellPosDd4hep(cellId))
    assert(gdmlPos == dd4hepPos)
