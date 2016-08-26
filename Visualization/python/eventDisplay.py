from __future__ import print_function
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
import argparse
assert -1 != ROOT.gSystem.Load("libDDEve"), "Could not load the Eve library! Check that libDDEve.so is in your path."
from ROOT import DD4hep


parser = argparse.ArgumentParser(description='Start the DD4Eve viewer.')
parser.add_argument("--config", "-c", help="DDEve Configuration XML file name.", default ="Visualization/compact/DDEve_FCCSW.xml")
parser.add_argument("--eventdata", "-e", help="FCCSW event data file name.", default="output.root")
args = parser.parse_args()


print("Starting DDEve Viewer...")
print("using DDEve Configuration XML file:  ", args.config)
print("using Event Data file:  ", args.eventdata)
dc = DD4hep.DisplayConfiguration # ??
DD4hep.EveDisplay(args.config, args.eventdata)
ROOT.gApplication.Run(1)
