import ROOT

rfile = ROOT.TFile.Open("./output.root", "READ")
events = rfile.Get("events")

assert(events.GetEntries() == 100)
