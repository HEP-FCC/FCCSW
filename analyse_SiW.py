from ROOT import TH1F, TTree, TChain, TFile, TF1, TCanvas, TGraphErrors, TPaveText
from array import array
import os
import numpy as np

FCCSW_DIR = "/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/"
ETAMIN = -0.001
ETAMAX = 0.001

RESOLUTION= {}
RESOLUTION_SIGMA = {}
LINEARITY = {}
LINEARITY_SIGMA = {}

CONFIGS = [ c for c in os.listdir(FCCSW_DIR+"/batch_eos/") if "50Layers_3.4mmPb" in c and "Pixels" not in c]
print CONFIGS

for CONFIG in CONFIGS:
   print CONFIG
 
   RUNCONFIGS = [ e for e in os.listdir(FCCSW_DIR+"/batch_eos/"+CONFIG)]
   for RUNCONFIG in RUNCONFIGS:
     #print "\t",RUNCONFIG

     dir="batch_eos/"+CONFIG+"/"+RUNCONFIG+"/"
     print "\n",dir

     chain = TChain("info")

     if os.path.isdir(dir):

       FILES = [f for f in os.listdir(dir) if "analogue"  in f and "root.root" not in f]
       for f in FILES:
          temp_file = TFile(dir+f)
          if temp_file.IsZombie():
            continue
          else:
            chain.Add(dir+f)
            #print f

     print chain.GetEntries()

     if chain.GetEntries() > 0:

       #schain.Print()

       chain.SetBranchStatus("*",0)
       chain.SetBranchStatus("edep_tot",1) # this is the number of particles per pixel summed in an event


       truth_energy = float(RUNCONFIG[:RUNCONFIG.find("GeV")])
       temp_edep = TH1F("edep_"+str(truth_energy)+"GeV", "Energy deposited per event; Energy Deposited [keV]; Count", 2000, 0, 500000)

       for i in np.arange(0,chain.GetEntries()):

          chain.GetEntry(i)
          temp_edep.Fill(chain.edep_tot);

       gaus = TF1("gaus","gaus",0,500000)
       temp_edep.Fit("gaus", "NQR")
       mean = gaus.GetParameter(1)
       sigma = gaus.GetParameter(2)

       if mean > 0:
         
        RESOLUTION[str(truth_energy)] =  sigma/ mean
        RESOLUTION_SIGMA[str(truth_energy)] = pow(pow(gaus.GetParError(1)/gaus.GetParameter(1),2) + pow(gaus.GetParError(2)/gaus.GetParameter(2),2),0.5) * RESOLUTION[str(truth_energy)]
        LINEARITY[str(truth_energy)] = mean
        LINEARITY_SIGMA[str(truth_energy)] = gaus.GetParError(1)
          
          
   print RESOLUTION

   res_plot = TGraphErrors(len(RESOLUTION),  array("d",[float(x) for x in RESOLUTION.keys()]),  
                                              array("d",[float(y) for y in RESOLUTION.values()]), 
                                              array("d",[float(0.1) for xe in RESOLUTION.keys()]),
                                              array("d",[float(ye) for ye in RESOLUTION_SIGMA.values()]))

    
   lin_plot = TGraphErrors(len(LINEARITY),   array("d",[float(x) for x in LINEARITY.keys()]),  
                                        array("d",[float(y) for y in LINEARITY.values()]), 
                                        array("d",[float(0.1) for xe in LINEARITY.keys()]),
                                        array("d",[float(ye) for ye in LINEARITY_SIGMA.values()]))

   c_res = TCanvas("Resolution")

   fit_min = 0
   fit_max = 1000
   fit = TF1("fit", "[0]/sqrt(x)+[1]", fit_min, fit_max)
   
   res_plot.SetMarkerStyle(22);
   res_plot.SetMaximum(0.08);
   res_plot.SetMinimum(0.0);
   res_plot.SetTitle("Resolution: 5x5mm Pads")
   res_plot.GetXaxis().SetTitle("Energy [GeV]")
   res_plot.GetYaxis().SetTitle("sigma_E / mu_E")
   res_plot.Fit(fit, "NR")
   fit.SetRange(0,1000);  
  
   res_plot.Draw("ap")
   fit.Draw("same")

   pt = TPaveText(0.55,0.55,0.8,0.85, "NDC")
   pt.SetBorderSize(0)
   pt.SetFillColor(0)
   pt.AddText('Fit Range: {fit_min} - {fit_max} '.format(fit_min=fit_min, fit_max=fit_max))
   #pt.AddText('#frac{{{noise:.1%}}}{{{E}}} #oplus #frac{{{stoch:.1%}}}{{{sqrt}}} #oplus {leakage:.1%} '.format(stoch=fit_0mm.GetParameter(0), sqrt="#sqrt{E}", leakage=fit_0mm.GetParameter(1),E="E", noise=fit_0mm.GetParameter(2)))
   pt.AddText('#frac{{{stoch:.1%}}}{{{sqrt}}} #oplus {leakage:.1%} '.format(stoch=fit.GetParameter(0), sqrt="#sqrt{E}", leakage=fit.GetParameter(1),E="E")) 
   pt.Draw()
   c_res.Print("SiW_300um_resolution.png")

   lin_plot.SetMarkerStyle(22)
   lin_plot.SetTitle("Linearity: ")
   lin_plot.GetXaxis().SetTitle("Energy [GeV]")
   lin_plot.GetYaxis().SetTitle("Energy Deposited [keV]")
   lin_fit = TF1("lin_fit", "pol1", fit_min, fit_max)
   lin_plot.Fit(lin_fit, "NR")
   lin_fit.SetRange(0,1000)

   c_lin = TCanvas("lin_plot")
   lin_plot.Draw("ap")
   lin_fit.Draw("same")
   pt_lin = TPaveText(0.25,0.7,0.5,0.85, "NDC")
   pt_lin.SetBorderSize(0)
   pt_lin.SetFillColor(0)
   #pt_lin.AddText('Fit Range: {fit_min} - {fit_max} '.format(fit_min, fit_max))
   #pt_lin.AddText('{x2:.4g}x^2 #plus {x:.4g}x #plus {c:.4g} '.format(x2=lin_fit.GetParameter(2), x=lin_fit.GetParameter(1), c=lin_fit.GetParameter(0)))
   #pt_lin.AddText(' {x:.4g}x #plus {c:.4g} '.format(x=lin_fit.GetParameter(1), c=lin_fit.GetParameter(0)))
   #pt_lin.Draw()
   c_lin.Print("SiW_300um_linearity.png")

closeInput = raw_input("Press ENTER to exit") 
