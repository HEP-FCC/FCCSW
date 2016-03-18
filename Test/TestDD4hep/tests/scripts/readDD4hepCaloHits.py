from ROOT import gSystem, gROOT, TCanvas, TH2D, gStyle
from EventStore import EventStore

gStyle.SetOptStat(0000)
gROOT.Reset()
c1 = TCanvas( 'Geant4', 'Geant4 cell IDs', 0, 0, 1900, 800 )
c1.Divide(2,1)
c2 = TCanvas( 'Geant4clusters', 'Geant4 cluster position', 0, 0, 1900, 800 )
c2.Divide(2,1)
cellNo = 13
hXY = TH2D('hXY','XY projection',cellNo,-0.5,cellNo-0.5,cellNo,-0.5,cellNo-0.5)
hXZ = TH2D('hYZ','YZ projection',cellNo,-0.5,cellNo-0.5,cellNo,-0.5,cellNo-0.5)
cXY = TH2D('cXY','XY projection',cellNo,-4.5,4.5,cellNo,-4.5,4.5)
cZY = TH2D('cZY','ZY projection',cellNo,-0.5,9.5,cellNo,-4.5,4.5)
gSystem.Load("libdatamodelDict")
store = EventStore(["1GeV//output_externalSD_8cm.root"])
for iev, event in enumerate(store):
    genptcs = event.get('allGenParticles')
    assert(len(genptcs)>0.)
    print(iev)
    print(len(genptcs))
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
    hits = event.get('hits')
    print("No of hits",len(hits))
    # for hit in hits:
        # print(hit.Core().Cellid)
        # x =
        # y =
        # z =
        # E = clu.Core().Energy
        # hXY.Fill(x,y,E)
        # hXZ.Fill(z,x,E)
    clusters = event.get('clusters')
    print(len(clusters))
    for clu in clusters:
        x = clu.Core().position.X/10.
        y = clu.Core().position.Y/10.
        z = clu.Core().position.Z/10.
        E = clu.Core().Energy # in GeV
        # print(x,y,z,E)
        cXY.Fill(x,y,E)
        cZY.Fill(z,y,E)
c2.cd(1)
cXY.Draw("colz")
cXY.GetXaxis().SetTitle("X (dm)")
cXY.GetYaxis().SetTitle("Y (dm)")
cXY.GetXaxis().CenterTitle()
cXY.GetYaxis().CenterTitle()
c2.cd(2)
cZY.Draw("colz")
cZY.GetXaxis().SetTitle("Z (dm)")
cZY.GetYaxis().SetTitle("X (dm)")
cZY.GetXaxis().CenterTitle()
cZY.GetYaxis().CenterTitle()
c2.Update()

c1.Print("hits.png")
c2.Print("1GeV/clusters_externalSD_8cm.png")
