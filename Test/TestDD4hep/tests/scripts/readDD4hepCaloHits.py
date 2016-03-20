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

def cell(cellId):
    id = [z(cellId), y(cellId), x(cellId)]
    print(id, bin(cellId))
    return cellId

from math import floor

def pos2cell(pos):
    return int(floor((pos+0.5*cellSize)/cellSize))

def z2cell(z):
    return int(floor((z-width/2.+0.5*cellSize)/cellSize))

from ROOT import gSystem, gROOT, TCanvas, TH2D, gStyle
from EventStore import EventStore

gStyle.SetOptStat(0000)
gROOT.Reset()
c1 = TCanvas( 'Geant4', 'Geant4 cell IDs', 0, 0, 1900, 800 )
c1.Divide(2,1)
c2 = TCanvas( 'Geant4clusters', 'Geant4 cluster position', 0, 0, 1900, 800 )
c2.Divide(2,1)
hXY = TH2D('hXY','XY projection',cellNo,-cellNo/2-0.5,cellNo/2+0.5,cellNo,-cellNo-0.5,cellNo+0.5)
hZY = TH2D('hZY','ZY projection',cellNo,-cellNo/2-0.5,cellNo/2+0.5,cellNo,-cellNo-0.5,cellNo+0.5)
cXY = TH2D('cXY','XY projection',cellNo,-cellNo/2-0.5,cellNo/2+0.5,cellNo,-cellNo-0.5,cellNo+0.5)
cZY = TH2D('cZY','ZY projection',cellNo,-cellNo/2-0.5,cellNo/2+0.5,cellNo,-cellNo-0.5,cellNo+0.5)
gSystem.Load("libdatamodelDict")
store = EventStore(["out_simpleTrackerSD_2cm.root"])
for iev, event in enumerate(store):
#    genptcs = event.get('allGenParticles')
#    assert(len(genptcs)>0.)
#    print(iev)
#    print(len(genptcs))
    # for ptc in genptcs:
        # print("px",ptc.Core().P4.Px)
        # print("py",ptc.Core().P4.Py)
        # print("pz",ptc.Core().P4.Pz)
        # print("status",ptc.Core().Status)
        # print("pdg",ptc.Core().Type)
        # print("x",ptc.Core().Vertex.X)
        # print("y",ptc.Core().Vertex.Y)
        # print("z",ptc.Core().Vertex.Z)
        # assert(ptc.Core().P4.Pz!=0.)
    hitsClus = event.get('hitClusterAssociation')
    print(len(hitsClus))
    for assoc in hitsClus:
        hit = assoc.Hit()
        clu = assoc.Cluster()
#    hits = event.get('hits')#caloHits')
# #    print("No of hits",len(hits))
#    for hit in hits:
        cellId = hit.Core().Cellid
        hxx = x(cellId)
        hyy = y(cellId)
        hzz = z(cellId)
#        print(cellId,xx,yy,zz)
        hE = hit.Core().Energy
        hXY.Fill(hxx,hyy,hE)
        hZY.Fill(hzz,hyy,hE)
#    clusters = event.get('clusters')#caloClusters')
# #    print(len(clusters))
#    for clu in clusters:
        cxx = pos2cell(clu.Core().position.X)
        cyy = pos2cell(clu.Core().position.Y)
        czz = z2cell(clu.Core().position.Z)
        cE = clu.Core().Energy # in GeV
#        print('cluster: ',clu.Core().position.Z,'     hit:  ',czz, bin(cellId) )
        cXY.Fill(cxx,cyy,cE)
        cZY.Fill(czz,cyy,cE)
        if(not(hxx == cxx )):#and hyy == cyy and hzz == czz)):
            print('x',clu.Core().position.X,hxx,cxx)
            # print('y',clu.Core().position.Y,hyy,cyy)
            # print('z',clu.Core().position.Z,hzz,czz)
            tmp=clu.Core().position.X+0.5*cellSize
            print(tmp)
            print(tmp/cellSize)
            print(floor(tmp/cellSize))
            print(int(floor(tmp/cellSize)))
            # print('E',hE,cE)
c1.cd(1)
hXY.Draw("colz")
hXY.GetXaxis().SetTitle("X cell no")
hXY.GetYaxis().SetTitle("Y cell no")
hXY.GetXaxis().CenterTitle()
hXY.GetYaxis().CenterTitle()
c1.cd(2)
hZY.Draw("colz")
hZY.GetXaxis().SetTitle("Z cell no")
hZY.GetYaxis().SetTitle("Y cell no")
hZY.GetXaxis().CenterTitle()
hZY.GetYaxis().CenterTitle()
c1.Update()

c2.cd(1)
cXY.Draw("colz")
cXY.GetXaxis().SetTitle("X cell no")
cXY.GetYaxis().SetTitle("Y cell no")
cXY.GetXaxis().CenterTitle()
cXY.GetYaxis().CenterTitle()
c2.cd(2)
cZY.Draw("colz")
cZY.GetXaxis().SetTitle("Z cell no")
cZY.GetYaxis().SetTitle("Y cell no")
cZY.GetXaxis().CenterTitle()
cZY.GetYaxis().CenterTitle()
c2.Update()

c1.Print("hits_externalSD_2cm.png")
c2.Print("clusters_externalSD_2cm.png")

raw_input("ddd")
