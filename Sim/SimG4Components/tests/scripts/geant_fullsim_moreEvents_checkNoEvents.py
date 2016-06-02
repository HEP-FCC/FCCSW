import ROOT

rfile = ROOT.TFile.Open("./out_full_moreEvents.root", "READ")
events = rfile.Get("events")

assert(events.GetEntries() == 10)
