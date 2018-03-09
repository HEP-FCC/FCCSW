from ROOT import TH1F, TTree, TChain, TFile, TGraphErrors, TF1
import numpy as np
import os
from array import array

FCCSW_DIR = "/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/"
ETAMIN = -0.001
ETAMAX = 0.001

CONFIGS = [ c for c in os.listdir(FCCSW_DIR+"/batch_eos/") if "50Layers_2.1mmW" in c and "Pixels" not in c]
print CONFIGS

PIXELS = {}
PIXELS_SIGMA = {}

for CONFIG in CONFIGS:
   print CONFIG
 
   RUNCONFIGS = [ e for e in os.listdir(FCCSW_DIR+"/batch_eos/"+CONFIG)  ]
   for RUNCONFIG in RUNCONFIGS:
     #print "\t",RUNCONFIG

     dir="batch_eos/"+CONFIG+"/"+RUNCONFIG+"/"
     print "\n",dir

     chain = TChain("decal_info")

     if os.path.isdir(dir):

       FILES = [f for f in os.listdir(dir) if "analysis" in f]
       for f in FILES:
          temp_file = TFile(dir+f)
          if temp_file.IsZombie():
            continue
          else:
            chain.Add(dir+f)

     print chain.GetEntries()

     temp_hist = TH1F("temp_hist", "temp hist", 2000, 0, 100000)#20)
     chain.Draw("pixels_tot>>temp_hist","", "GOFF" )

     energy = float(RUNCONFIG[:RUNCONFIG.find("GeV")])
     mean = temp_hist.GetMean()
     sigma = temp_hist.GetRMS()
     if mean > 0:
       print energy, mean, sigma, sigma/mean
       PIXELS[str(energy)] = mean
       PIXELS_SIGMA[str(energy)] = sigma / np.sqrt(chain.GetEntries())

print PIXELS
lin_plot = TGraphErrors(len(PIXELS),   array("d",[float(x) for x in PIXELS.keys()]),  
                                      array("d",[float(y) for y in PIXELS.values()]), 
                                      array("d",[float(0.1) for xe in PIXELS.keys()]),
                                      array("d",[float(ye) for ye in PIXELS_SIGMA.values()]))

fit = TF1("fit", "pol2", 0, 1000)
lin_plot.Fit(fit)
lin_plot.Draw("ape")
closeInput = raw_input("Press ENTER to exit") 
