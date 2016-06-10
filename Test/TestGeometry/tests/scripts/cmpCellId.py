cellNo = 201
cellSize = 5
bitfieldsize = 8
signed = True

def retrieve(no, mask, offset):
    id = (no & mask)
    id = id >> offset
    if(signed):
        id = id - 2**bitfieldsize if id >= 2**(bitfieldsize-1) else id
    return id

def z(cellId):
    return retrieve(cellId, 0b000000000000000011111111,0)

def y(cellId):
    return retrieve(cellId, 0b000000001111111100000000,1*bitfieldsize)

def x(cellId):
    return retrieve(cellId, 0b111111110000000000000000,2*bitfieldsize)

def cellIdDd4hep(cellId):
    return array('d',(x(cellId),y(cellId),z(cellId)))

def cellPosDd4hep(cellId):
    return array('d',(x(cellId)*cellSize,y(cellId)*cellSize,(cellNo/2.+z(cellId))*cellSize))

def cellIdGdml(cellId):
    z = cellId%cellNo
    x = cellId/(cellNo*cellNo)
    y = cellId/cellNo%cellNo
    return array('d',(x,y,z))

def cellPosGdml(cellId):
    z = (cellId%cellNo+0.5)*cellSize
    x = ((cellId/(cellNo*cellNo))-cellNo/2)*cellSize
    y = ((cellId/cellNo%cellNo)-cellNo/2)*cellSize
    return array('d',(x,y,z))

def cmpId(cellIdGdml, cellIdDd4hep):
    IdGdml = cellIdGdml(cellIdGdml)
    IdDd4hep = cellIdDd4hep(cellIdDd4hep)
    return bool(IdGdml == IdDd4hep)

from ROOT import gSystem
from EventStore import EventStore
from array import array
import os
from math import sqrt, pow

gSystem.Load("libdatamodelDict")
en = 1
storeGdml = EventStore(["testCellId_gdml.root"])
storeDD4hep = EventStore(["testCellId_dd4hep_segmentation.root"])

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
gdmlPos = []
for iev, event in enumerate(storeGdml):
    hits = event.get('caloHits')
    for hit in hits:
        cellId = hit.Core().Cellid
        # histGdml.Fill(cellPosGdml(cellId))
        gdmlPos.append(cellPosGdml(cellId))
        print(" event ",iev)
        print("======GDML : id: ",cellIdGdml(cellId)," -> pos: ",cellPosGdml(cellId))
print(gdmlPos)
print("======DD4hep===========")
dd4hepPos = []
for iev, event in enumerate(storeDD4hep):
    hits = event.get('caloHits')
    for hit in hits:
        cellId = hit.Core().Cellid
        # histDd4hep.Fill(cellPosDd4hep(cellId))
        dd4hepPos.append(cellPosDd4hep(cellId))
        print(" event ",iev)
        print("======DD4hep : id: ",cellIdDd4hep(cellId)," -> pos: ",cellPosDd4hep(cellId))
print(dd4hepPos)
assert(gdmlPos == dd4hepPos)
print("=======================")
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
