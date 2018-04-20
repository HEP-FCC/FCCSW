"""
This script separates FCC vertices in start and end, (and primary and secondary, if information available)
and writes them to separate branches in a new files, creating graphs for the positions.
"""


from EventStore import EventStore
import argparse
import ROOT
from ROOT import fcc, std
import numpy as np


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

print "creating root file and trees ..."
f = ROOT.TFile(args.output, "recreate")
t = ROOT.TTree( 'processedEvents', 'tree with processed fcc data' )
psvb = t.Branch("primaryStartVertices", primaryStartVertexVector)
pevb = t.Branch("primaryEndVertices", primaryEndVertexVector)
ssvb = t.Branch("secondaryStartVertices", secondaryStartVertexVector)
sevb = t.Branch("secondaryEndVertices", secondaryEndVertexVector)



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
  simparticles = store.get("simParticles")
  for p in simparticles:
    svertex = p.startVertex()
    evertex = p.endVertex()
    StartVertex = fcc.GenVertexData()
    StartVertex.position = svertex.position()
    StartVertex.ctau = svertex.ctau()
    EndVertex = fcc.GenVertexData()
    EndVertex.position = evertex.position()
    EndVertex.ctau = evertex.ctau()
    if p.status() == 201: # secondary
      secondaryStartVertexVector.push_back(StartVertex)
      secondaryEndVertexVector.push_back(EndVertex)

    else: # primary
      primaryStartVertexVector.push_back(StartVertex)
      primaryEndVertexVector.push_back(EndVertex)

  t.Fill()
  primaryEndVertexVector.clear()
  primaryStartVertexVector.clear()
  secondaryEndVertexVector.clear()
  secondaryStartVertexVector.clear()

print "... finished event loop"

ROOT.gROOT.SetBatch()
f.cd()
for _ptype in ["primary", "secondary"]:
  for _startend in ["Start", "End"]:
    

    t.Draw(_ptype+_startend+"Vertices.position.x:"+_ptype+_startend+"Vertices.position.y")
    graph_xy = ROOT.TGraph(t.GetSelectedRows(), t.GetV2(), t.GetV1());
    graph_xy.SetMarkerStyle(4)
    graph_xy.SetTitle("SimParticles: "+_ptype+" "+_startend+" vertices")
    graph_xy.GetXaxis().SetTitle("X [mm]")
    graph_xy.GetYaxis().SetTitle("Y [mm]")


    graph_xy.SetLineColorAlpha(ROOT.kWhite, 1.0);
    graph_xy.SetMarkerColor(ROOT.kBlue);
    graph_xy.Write(graph_xy.GetTitle())

    t.Draw("sqrt(pow("+_ptype+_startend+"Vertices.position.x,2)+pow("+_ptype+_startend+"Vertices.position.y,2)):"+_ptype+_startend+"Vertices.position.z")
    graph_rz = ROOT.TGraph(t.GetSelectedRows(), t.GetV2(), t.GetV1());
    graph_rz.SetMarkerStyle(4)
    graph_rz.SetTitle("SimParticles: "+ _ptype + " " + _startend + " vertices")
    graph_rz.GetXaxis().SetTitle("Z [mm]")
    graph_rz.GetYaxis().SetTitle("R [mm]")
    graph_rz.SetLineColorAlpha(ROOT.kWhite, 1.0);
    graph_rz.SetMarkerColor(ROOT.kBlue);
    graph_rz.Write(graph_rz.GetTitle())

print "... writing root file " + args.output
t.Write()

