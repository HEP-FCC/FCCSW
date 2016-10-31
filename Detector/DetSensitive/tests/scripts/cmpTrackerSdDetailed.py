from ROOT import gSystem, THnSparseD
from EventStore import EventStore
from array import array

bitfieldsize = 6
cellNo = 51
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

def cell(cellId):
    return array('d',(x(cellId),y(cellId),z(cellId)))

if __name__ == "__main__":
    gSystem.Load("libdatamodelDict")
    store1 = EventStore(["./out_dd4hepTrackerSD_2cm.root"])
    store2 = EventStore(["./out_simpleTrackerSD_2cm.root"])
    bins = array('i',(cellNo, cellNo, cellNo))
    axisMin = array('d',(-cellNo/2-0.5, -cellNo/2-0.5, -cellNo/2-0.5))
    axisMax = array('d',(cellNo/2+0.5, cellNo/2+0.5, cellNo/2+0.5))
    hist1 = THnSparseD('hist1','hist1', 3, bins, axisMin, axisMax)
    hist2 = THnSparseD('hist2','hist2', 3, bins, axisMin, axisMax)
    for iev, event in enumerate(store1):
        hits = event.get('hits')
        for hit in hits:
            cellId = hit.cellId()
            hE = hit.energy()
            hist1.Fill(cell(cellId),hE)
    for iev, event in enumerate(store2):
        hits = event.get('hits')
        for hit in hits:
            cellId = hit.cellId()
            hE = hit.energy()
            hist2.Fill(cell(cellId),hE)
    for ix in range(cellNo):
        for iy in range(cellNo):
            for iz in range(cellNo):
                assert(hist1.GetBinContent(array('i',(ix,iy,iz))) == hist2.GetBinContent(array('i',(ix,iy,iz))))
