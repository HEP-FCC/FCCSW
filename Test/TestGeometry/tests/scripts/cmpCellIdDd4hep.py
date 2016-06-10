cellNo = 201
cellSize = 5
bitfieldsize = 8

def retrieve(sign, no, mask, offset):
    id = (no & mask)
    id = id >> offset
    if(sign):
        id = id - 2**bitfieldsize if id >= 2**(bitfieldsize-1) else id
    return id

def z(cellId, sign):
    return retrieve(sign, cellId, 0b000000000000000011111111,0)

def y(cellId, sign):
    return retrieve(sign, cellId, 0b000000001111111100000000,1*bitfieldsize)

def x(cellId, sign):
    return retrieve(sign, cellId, 0b111111110000000000000000,2*bitfieldsize)

def cellPosDd4hep(cellId, sign):
    if(sign):
        return array('d',(x(cellId, sign)*cellSize,y(cellId, sign)*cellSize,(cellNo/2.+z(cellId, sign))*cellSize))
    return array('d',((x(cellId, sign)-cellNo/2)*cellSize,
                      (y(cellId, sign)-cellNo/2)*cellSize,
                      (1./2.+z(cellId, sign))*cellSize))

from ROOT import gSystem
from EventStore import EventStore
from array import array
import os
from math import sqrt, pow

gSystem.Load("libdatamodelDict")
en = 1
storeSeg = EventStore(["testCellId_dd4hep_segmentation.root"])
storeVol = EventStore(["testCellId_dd4hep_volumes.root"])

# bins = array('i',(cellNo, cellNo, cellNo))
# axisMin = array('d',(-cellNo*cellSize/2., -cellNo*cellSize/2., 0))
# axisMax = array('d',(cellNo*cellSize/2., cellNo*cellSize/2., cellNo*cellSize))
# print ("bins no = ",bins)
# print ("axes min = ",axisMin)
# print ("axes max = ",axisMax)
# histGdml = THnSparseD('histGdml','histGdml', 3, bins, axisMin, axisMax)
# histDd4hep = THnSparseD('histDd4hep','histDd4hep', 3, bins, axisMin, axisMax)

# print("======test gdml formulas============")
# testcellId = cellNo*cellNo*0+cellNo*0+0
# print("0,0,0 -> id ",cellIdGdml(testcellId), " -> pos: ",cellPosGdml(testcellId))
# testcellId = cellNo*cellNo*200+cellNo*200+200
# print("200,200,200 -> id ",cellIdGdml(testcellId), " -> pos: ",cellPosGdml(testcellId))
# testcellId = cellNo*cellNo*100+cellNo*100+100
# print("100,100,100 -> id ",cellIdGdml(testcellId), " -> pos: ",cellPosGdml(testcellId))

# print("======test dd4hep formulas============")
# print("=======================")

print("======GDML============")
segPos = []
for iev, event in enumerate(storeSeg):
    hits = event.get('caloHits')
    for hit in hits:
        cellId = hit.Core().Cellid
        # histGdml.Fill(cellPosGdml(cellId))
        segPos.append(cellPosDd4hep(cellId,True))
        print(" event ",iev)
        print("======DD4hep segmentation:  pos: ",cellPosDd4hep(cellId,True))
print(segPos)
print("======DD4hep===========")
volPos = []
for iev, event in enumerate(storeVol):
    hits = event.get('caloHits')
    for hit in hits:
        cellId = hit.Core().Cellid
        # histDd4hep.Fill(cellPosDd4hep(cellId))
        volPos.append(cellPosDd4hep(cellId,False))
        print(" event ",iev)
        print("======DD4hep volumes : pos: ",cellPosDd4hep(cellId,False))
print(volPos)
print("=======================")
for i, val in enumerate(segPos):
    if (segPos[i] != volPos[i]):
        print(segPos[i]," != ", volPos[i])
    else:
        print("=")
assert(segPos == volPos)
# tmp printing
# hXYGdml = histGdml.Projection(1,0)
# hZYGdml = histGdml.Projection(1,2)
# hXYDd4hep = histDd4hep.Projection(1,0)
# hZYDd4hep = histDd4hep.Projection(1,2)
# c = TCanvas( "testCellId", "testCellId", 0, 0, 1920, 1050 )
# c.Divide(2,2)
# c.cd(1)
# hXYGdml.Draw("colz")
# c.cd(2)
# hZYGdml.Draw("colz")
# c.cd(3)
# hXYDd4hep.Draw("colz")
# c.cd(4)
# hZYDd4hep.Draw("colz")
# c.Print("cmp_cellId.png")

# histGdml.Add(histDd4hep,-1.)
# assert(histGdml.GetEntries()==0)
