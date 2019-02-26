

from EventStore import EventStore
import argparse
import ROOT
from ROOT import fcc, std
import numpy as np


# command line arguments
parser = argparse.ArgumentParser()
parser.add_argument("filename", help="edm file to read")
parser.add_argument("--nevents", help="max events to process (takes length of input root file by default)", type=int, default=-1)
parser.add_argument('--output', type=str, help="name of rootfile to write", default="ExtrapolationCheckPlots.root")
args = parser.parse_args()


print "creating root file and trees ..."
f = ROOT.TFile(args.filename, "open")
outfile = ROOT.TFile(args.output, "recreate")
t = f.Get("events")

ROOT.gROOT.SetBatch()
outfile.cd()
    

c1 = ROOT.TCanvas()

for _htype in ["SimTrackerPositionedHits.position", "ExtrapolatedTrackstates.referencePoint"]:
  t.Draw(_htype+".x:"+_htype+".y")
  graph_xy = ROOT.TGraph(t.GetSelectedRows(), t.GetV2(), t.GetV1());
  graph_xy.SetMarkerStyle(4)
  graph_xy.SetTitle(_htype + " xy")
  graph_xy.GetXaxis().SetTitle("X [mm]")
  graph_xy.GetYaxis().SetTitle("Y [mm]")


  graph_xy.SetLineColorAlpha(ROOT.kWhite, 1.0);
  graph_xy.SetMarkerColor(ROOT.kBlue);
  graph_xy.Write(graph_xy.GetTitle())

  t.Draw("sqrt(pow("+_htype+".x,2)+pow("+_htype+".y,2)):"+_htype+".z")
  graph_rz = ROOT.TGraph(t.GetSelectedRows(), t.GetV2(), t.GetV1());
  graph_rz.SetMarkerStyle(4)
  graph_rz.SetTitle(_htype + " rz")
  graph_rz.GetXaxis().SetTitle("Z [mm]")
  graph_rz.GetYaxis().SetTitle("R [mm]")
  graph_rz.SetLineColorAlpha(ROOT.kWhite, 1.0);
  graph_rz.SetMarkerColor(ROOT.kBlue);
  graph_rz.Write(graph_rz.GetTitle())

  t.Draw("@"+_htype.split(".")[0]+".size():atanh(allGenParticles.core.p4.pz / sqrt(pow(allGenParticles.core.p4.px,2) + pow(allGenParticles.core.p4.py,2) + pow(allGenParticles.core.p4.pz,2)))")
  graph_sizeEta = ROOT.TGraph(t.GetSelectedRows(), t.GetV2(), t.GetV1());
  graph_sizeEta.SetMarkerStyle(4)
  graph_sizeEta.SetTitle(_htype + " etaSize")
  graph_sizeEta.GetXaxis().SetTitle("X [mm]")
  graph_sizeEta.GetYaxis().SetTitle("Y [mm]")


  graph_sizeEta.SetLineColorAlpha(ROOT.kWhite, 1.0);
  graph_sizeEta.SetMarkerColor(ROOT.kBlue);
  graph_sizeEta.Write(graph_xy.GetTitle())
  
  h2 = ROOT.TH2F(_htype+"numSize2D", _htype+"numSize2D", 100, 0., 6., 100, 0., 29.5)
  t.Project(_htype+"numSize2D", "@"+_htype.split(".")[0]+".size():atanh(allGenParticles.core.p4.pz / sqrt(pow(allGenParticles.core.p4.px,2) + pow(allGenParticles.core.p4.py,2) + pow(allGenParticles.core.p4.pz,2)))")

  pf1 = h2.ProfileX(_htype+"Profile")
  pf1.Write()
  h2.Write()

print "... writing root file " + args.output
