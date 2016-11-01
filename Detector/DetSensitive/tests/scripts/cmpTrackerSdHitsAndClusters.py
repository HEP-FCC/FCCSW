from ROOT import gSystem, gROOT, TCanvas, TH2D, gStyle
from EventStore import EventStore

bitfieldsize = 6
cellNo = 51
width = 1020
cellSize = width/cellNo
signed = True

def retrieve(no, mask, offset):
    id = (no & mask)
    id = id >> offset
    if(signed):
        id = id - 2**bitfieldsize if id >= 2**(bitfieldsize-1) else id
    return id

def z(cellId):
    return retrieve(cellId, 0b000000000000111111,0)

def y(cellId):
    return retrieve(cellId, 0b000000111111000000,1*bitfieldsize)

def x(cellId):
    return retrieve(cellId, 0b111111000000000000,2*bitfieldsize)

from math import floor

def pos2cell(pos):
    return int(floor((pos+0.5*cellSize)/cellSize))

def z2cell(z):
    return int(floor((z-width/2.+0.5*cellSize)/cellSize))


if __name__ == "__main__":
    gSystem.Load("libdatamodelDict")
    store = EventStore(["./out_simpleTrackerSD_2cm.root"])
    for iev, event in enumerate(store):
        posHits = event.get('positionedHits')
        print(len(posHits))
        for hit in posHits:
            cellId = hit.cellId()
            hxx = x(cellId)
            hyy = y(cellId)
            hzz = z(cellId)
            hE = hit.energy()
            cxx = pos2cell(hit.position().x)
            cyy = pos2cell(hit.position().y)
            czz = z2cell(hit.position().z)
            cE = hit.energy()
            assert(hxx == cxx)
            assert(hyy == cyy)
            assert(hzz == czz)
            assert(hE == cE)
