"""
This script separates FCC vertices in start and end, (and primary and secondary, if information available)
and writes them to separate branches in a new files, creating graphs for the positions.
"""


from EventStore import EventStore
import argparse
import ROOT
from ROOT import fcc, std
import numpy as np
import networkx as nx


# command line arguments
parser = argparse.ArgumentParser()
parser.add_argument("filename", help="edm file to read")
parser.add_argument("--nevents", help="max events to process (takes length of input root file by default)", type=int, default=-1)
parser.add_argument('--output', type=str, help="name of rootfile to write", default="simVertices.root")
args = parser.parse_args()



primaryStartVertexVector = std.vector(fcc.GenVertexData)()
primaryEndVertexVector = std.vector(fcc.GenVertexData)()
secondaryStartVertexVector = std.vector(fcc.GenVertexData)()
secondaryEndVertexVector = std.vector(fcc.GenVertexData)()
hitProducingStartVertices = std.vector(fcc.GenVertexData)()

print "creating root file and trees ..."
f = ROOT.TFile(args.output, "recreate")
t = ROOT.TTree( 'processedEvents', 'tree with processed fcc data' )
psvb = t.Branch("primaryStartVertices", primaryStartVertexVector)
pevb = t.Branch("primaryEndVertices", primaryEndVertexVector)
ssvb = t.Branch("secondaryStartVertices", secondaryStartVertexVector)
sevb = t.Branch("secondaryEndVertices", secondaryEndVertexVector)
hb = t.Branch("hitProducingStartVertices", hitProducingStartVertices)

def r(point):
  return np.sqrt(point.x**2 + point.y**2 + point.z**2)



events = EventStore([args.filename])
if args.nevents == -1:
  args.nevents=len(events)
print len(events), " events in rootfile ", args.filename

j = 0
k = 0
print ""
for i, store in enumerate(events):
  print ".",
  if i > args.nevents: 
    break
  trackId2Vertex = {}
  trackId2Particle = {}
  graph = nx.DiGraph()
  simparticles = store.get("simParticles")
  for p in simparticles:
    pclone = fcc.MCParticle()
    pclone.core(p.core())
    pclone.vertex(p.startVertex().position())
    """
    vclone = fcc.GenVertex()
    vclone.position(p.startVertex().position())
    vclone.ctau(p.startVertex().ctau())
    pclone.startVertex(vclone())
    """
    trackId2Particle[p.bits()] = pclone
    print p.core().status, p.bits() 
    graph.add_edge(p.core().status, p.bits())

    svertex = p.startVertex()
    evertex = p.endVertex()
    StartVertex = fcc.GenVertexData()
    StartVertex.position = svertex.position()
    StartVertex.ctau = svertex.ctau()
    EndVertex = fcc.GenVertexData()
    EndVertex.position = evertex.position()
    EndVertex.ctau = evertex.ctau()
    trackId2Vertex[p.bits()] = StartVertex
    if p.status() > 0: # secondary
      secondaryStartVertexVector.push_back(StartVertex)
      secondaryEndVertexVector.push_back(EndVertex)

    else: # primary
      primaryStartVertexVector.push_back(StartVertex)
      primaryEndVertexVector.push_back(EndVertex)
  trackerhits = store.get("positionedHits")
  for h in trackerhits:
      print "hit trackId: ", h.bits()
      for a in nx.ancestors(graph, h.bits()):
            try:
              c = trackId2Particle[a].core()
              v = c.vertex
              print  "\t trackId: ", a, "\t pdgId: ", c.pdgId, "\t momentum: ", c.p4.px, c.p4.py, c.p4.pz, "\t startvertex: ", v.x, v.y, v.z
            except:
              print "reached primary: ", a
      if h.cellId() % 32 == 0 and h.cellId() / 32 % 32 == 0:
        hit_trackId = h.bits()
        hitProducer_startVertex = trackId2Vertex[hit_trackId]
        hitProducingStartVertices.push_back(hitProducer_startVertex)

  t.Fill()
  primaryEndVertexVector.clear()
  primaryStartVertexVector.clear()
  secondaryEndVertexVector.clear()
  secondaryStartVertexVector.clear()
  hitProducingStartVertices.clear()
  


print "... finished event loop"

ROOT.gROOT.SetBatch()
f.cd()
for branchname in ["primaryStartVertices", "primaryEndVertices", "secondaryStartVertices", "secondaryEndVertices", "hitProducingStartVertices"]:
    

    t.Draw(branchname+".position.x:"+branchname+".position.y")
    graph_xy = ROOT.TGraph(t.GetSelectedRows(), t.GetV2(), t.GetV1());
    graph_xy.SetMarkerStyle(4)
    graph_xy.SetTitle("SimParticles: " + branchname)
    graph_xy.GetXaxis().SetTitle("X [mm]")
    graph_xy.GetYaxis().SetTitle("Y [mm]")


    graph_xy.SetLineColorAlpha(ROOT.kWhite, 1.0);
    graph_xy.SetMarkerColor(ROOT.kBlue);
    graph_xy.Write(graph_xy.GetTitle())

    t.Draw("sqrt(pow("+branchname+".position.x,2)+pow("+branchname+".position.y,2)):"+branchname+".position.z")
    graph_rz = ROOT.TGraph(t.GetSelectedRows(), t.GetV2(), t.GetV1());
    graph_rz.SetMarkerStyle(4)
    graph_rz.SetTitle("SimParticles: "+ branchname)
    graph_rz.GetXaxis().SetTitle("Z [mm]")
    graph_rz.GetYaxis().SetTitle("R [mm]")
    graph_rz.SetLineColorAlpha(ROOT.kWhite, 1.0);
    graph_rz.SetMarkerColor(ROOT.kBlue);
    graph_rz.Write(graph_rz.GetTitle())

    t.Draw(branchname+".position.x:" + branchname + ".position.y")
    graph_xy = ROOT.TGraph(t.GetSelectedRows(), t.GetV2(), t.GetV1());
    graph_xy.SetMarkerStyle(4)
    graph_xy.SetTitle("SimParticles: " + branchname)
    graph_xy.GetXaxis().SetTitle("X [mm]")
    graph_xy.GetYaxis().SetTitle("Y [mm]")


    graph_xy.SetLineColorAlpha(ROOT.kWhite, 1.0);
    graph_xy.SetMarkerColor(ROOT.kBlue);
    graph_xy.Write(graph_xy.GetTitle())

    t.Draw("sqrt(pow("+branchname+".position.x,2)+pow("+branchname+".position.y,2)):"+branchname+".position.z")
    graph_rz = ROOT.TGraph(t.GetSelectedRows(), t.GetV2(), t.GetV1());
    graph_rz.SetMarkerStyle(4)
    graph_rz.SetTitle("SimParticles: " + branchname)
    graph_rz.GetXaxis().SetTitle("Z [mm]")
    graph_rz.GetYaxis().SetTitle("R [mm]")
    graph_rz.SetLineColorAlpha(ROOT.kWhite, 1.0);
    graph_rz.SetMarkerColor(ROOT.kBlue);
    graph_rz.Write(graph_rz.GetTitle())



print "... writing root file " + args.output
t.Write()

