from __future__ import print_function

import ROOT

rfile = ROOT.TFile.Open("./output.root", "READ")
events = rfile.Get("events")

assert(events.GetEntries() == 10)
