import ROOT

rfile = ROOT.TFile.Open("test_geant_fullsim_moreEvents.root", "READ")
events = rfile.Get("events")

assert(events.GetEntries() == 10)
