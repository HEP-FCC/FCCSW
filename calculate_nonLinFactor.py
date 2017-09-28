from ROOT import TH1F, TTree, TChain, TFile
import os

FCCSW_DIR = "/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/"
ETAMIN = -0.001
ETAMAX = 0.001

CONFIGS = [ c for c in os.listdir(FCCSW_DIR+"/batch_eos/") if "50Layers_2.1mmW_50umPixels_18umThick" in c]
print CONFIGS

for CONFIG in CONFIGS:
   print CONFIG
 
   RUNCONFIGS = [ e for e in os.listdir(FCCSW_DIR+"/batch_eos/"+CONFIG)  ]
   for RUNCONFIG in RUNCONFIGS[1:]:
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

