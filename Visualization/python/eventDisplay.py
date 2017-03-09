from __future__ import print_function
import os
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
import argparse


# fix from DDG4 for TLS error on lxplus:
# Try to load libglapi to avoid issues with TLS Static
# Turn off all errors from ROOT about the library missing
orgLevel = ROOT.gErrorIgnoreLevel
ROOT.gErrorIgnoreLevel=6000
ROOT.gSystem.Load("libglapi")
ROOT.gSystem.Load("libQtOpenGL")
ROOT.gErrorIgnoreLevel=orgLevel


assert -1 != ROOT.gSystem.Load("libDDEve"), "Could not load the Eve library! Check that libDDEve.so is in your path."
from ROOT import DD4hep


parser = argparse.ArgumentParser(description='Start the DD4Eve viewer.')
parser.add_argument("--config", "-c", help="DDEve Configuration XML file name.", default ="Visualization/compact/DDEve_FCCSW.xml")
parser.add_argument("--eventdata", "-e", help="FCCSW event data file name.", default="output.root")
args = parser.parse_args()


print("Starting DDEve Viewer...")
print("using DDEve Configuration XML file:  ", args.config)
print("using Event Data file:  ", args.eventdata)
assert os.path.isfile(args.eventdata), "File " + args.eventdata + " does not exist!"
dc = DD4hep.DisplayConfiguration # ?? EveDisplay can only be accessed in python if some other DD4hep variable is accessed first
DD4hep.EveDisplay(args.config, args.eventdata)
ROOT.gApplication.Run(1)
