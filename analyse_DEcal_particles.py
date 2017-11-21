from ROOT import TH1F, TTree, TChain, TFile, TF1, TH2F, TGraphErrors, TCanvas, TPaveText
import os
import numpy as np
from array import array


FCCSW_DIR = "/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/"
ETAMIN = -0.001
ETAMAX = 0.001

p0 = 3.28652e+02
p1 = 9.89744e+01
p2 = -1.25638e-02

calibration = TF1("calibration", "pol2", 0, 3000)
calibration.SetParameter("p0", p0)
calibration.SetParameter("p1", p1)
calibration.SetParameter("p2", p2)

gaus = TF1("gaus", "gaus", 0, 3000)

RESOLUTION_PART = {}
RESOLUTION_PART_SIGMA = {}
LINEARITY_PART = {}
LINEARITY_PART_SIGMA = {}

RESOLUTION = {}
RESOLUTION_SIGMA = {}
LINEARITY = {}
LINEARITY_SIGMA = {}

CONFIGS = ["50Layers_2.1mmW_FCCSW0.8"] #[ c for c in os.listdir(FCCSW_DIR+"/batch_eos/") if "50Layers_2.1mmW_50umPixels_18umThick" in c and "Edep" not in c]
print CONFIGS

for CONFIG in CONFIGS:
   print CONFIG

   fout = TFile(CONFIG+".root", "RECREATE")
   fout.cd()
   partperpixvspix = TH2F("partperpixvspix", ";Pixels per event;Particles / Pixel; Count", 2000,0,120000,100, 0.8,1.4)
 
   RUNCONFIGS = [ e for e in os.listdir(FCCSW_DIR+"/batch_eos/"+CONFIG)  ]
   for RUNCONFIG in RUNCONFIGS:
     #print "\t",RUNCONFIG

     dir="batch_eos/"+CONFIG+"/"+RUNCONFIG+"/"
     #print "\n",dir

     chain = TChain("decal_info")

     if os.path.isdir(dir):

       FILES = [f for f in os.listdir(dir) if  "analysis_"  in f and "root.root" not in f]
       for f in FILES:
          temp_file = TFile(dir+f)
          if temp_file.IsZombie():
            continue
          else:
            chain.Add(dir+f)

     if chain.GetEntries() > 0:

       #schain.Print()

       chain.SetBranchStatus("*",0)
       chain.SetBranchStatus("particles_tot",1) # this is the number of particles per pixel summed in an event
       chain.SetBranchStatus("pixels_tot",1)
       chain.SetBranchStatus("particles_l*", 1)
       chain.SetBranchStatus("pixels_l*",1)

       truth_energy = float(RUNCONFIG[:RUNCONFIG.find("GeV")])

       temp_energy = TH1F("energy_"+str(truth_energy)+"GeV", "Reconstructed energy from pol2; E_{truth}; E_{reco}", 2000, 0, 1200)
       temp_particles = TH1F("particles_"+str(truth_energy)+"GeV", "Particles per event; Particles per event; Count", 2000, 0, 120000)#20)
       temp_pixels = TH1F("pixels_"+str(truth_energy)+"GeV", "Pixels per event; Pixels per event; Count", 2000, 0, 120000)
       temp_partvspix = TH2F("partvspix_"+str(truth_energy)+"GeV", ";Pixels per event; Particles per event", 2000,0,120000,2000,0,120000)
       temp_partperpix = TH1F("partperpix_"+str(truth_energy)+"GeV", ";Particles / Pixel; Count", 100, 0.8, 1.4)
       temp_partperpixvspix = TH2F("partperpixvspix_"+str(truth_energy)+"GeV", ";Pixels per event;Particles / Pixel; Count", 2000,0,120000,100, 0.8, 1.4)

       for i in np.arange(0,chain.GetEntries()):

          chain.GetEntry(i)
          pixels_tot = chain.pixels_tot
          particles_tot = chain.particles_tot
          
          temp_particles.Fill(particles_tot)
          temp_pixels.Fill(pixels_tot)
          temp_partvspix.Fill(pixels_tot,particles_tot)
          temp_partperpix.Fill(particles_tot/pixels_tot)
          temp_partperpixvspix.Fill(pixels_tot, particles_tot/pixels_tot)
          partperpixvspix.Fill(pixels_tot, particles_tot/pixels_tot)

          energy = calibration.GetX(pixels_tot)
          temp_energy.Fill(energy)

          #loop through the layers to get particles / pixel
          

          #if i == 0 or i%100==0:
          #  print i, "/", chain.GetEntries()
          #  print "\t", pixels_tot, energy
 

       #temp_partperpix.Draw("")
       #temp_ereco.Fit("gaus","N")
       
       fout.cd()

       # fit info for the particles
       gaus.SetRange(0,120000)
       temp_particles.Fit("gaus", "NQR")
       temp_particles.Write()
       mean_particles = gaus.GetParameter(1)
       sigma_particles = gaus.GetParameter(2)

       if mean_particles > 0:
         
        RESOLUTION_PART[str(truth_energy)] =  sigma_particles/ mean_particles
        RESOLUTION_PART_SIGMA[str(truth_energy)] = pow(pow(gaus.GetParError(1)/gaus.GetParameter(1),2) + pow(gaus.GetParError(2)/gaus.GetParameter(2),2),0.5) * RESOLUTION_PART[str(truth_energy)]
        LINEARITY_PART[str(truth_energy)] = mean_particles
        LINEARITY_PART_SIGMA[str(truth_energy)] = gaus.GetParError(1)

       # fit info for the pixels
       gaus.SetRange(0,120000)
       temp_pixels.Fit("gaus", "NQR")
       temp_pixels.Write()
       mean_pixels = gaus.GetParameter(1)
       sigma_pixels = gaus.GetParameter(2)

       if mean_pixels > 0:
         
        RESOLUTION[str(truth_energy)] =  sigma_pixels/ mean_pixels
        RESOLUTION_SIGMA[str(truth_energy)] = pow(pow(gaus.GetParError(1)/gaus.GetParameter(1),2) + pow(gaus.GetParError(2)/gaus.GetParameter(2),2),0.5) * RESOLUTION[str(truth_energy)]
        LINEARITY[str(truth_energy)] = mean_pixels
        LINEARITY_SIGMA[str(truth_energy)] = gaus.GetParError(1)

       # fit info for the calibrated energy
       gaus.SetRange(0,3000)
       temp_energy.Fit("gaus", "NQR")
       temp_energy.Write()
       mean_ereco = gaus.GetParameter(1)
       sigma_ereco = gaus.GetParameter(2)

       temp_partperpix.Write()
       temp_partvspix.Write()
       temp_partperpixvspix.Write()

       if mean_particles > 0:
         
         print "\n\nTruth Energy = ", truth_energy
         print "nEvts = ", chain.GetEntries()
         print "Particles = ", mean_particles, "+/-", sigma_particles
         print "Pixels = ", mean_pixels, "+/-", sigma_pixels
         print "Ereco = ", mean_ereco, "+/-", sigma_ereco
        # print "Reco Energy = ", mean, "+/-", sigma
        # print "Resolution = ", sigma/mean



   partperpixvspix.Write()

   res_plot = TGraphErrors(len(RESOLUTION),  array("d",[float(x) for x in RESOLUTION.keys()]),  
                                            array("d",[float(y) for y in RESOLUTION.values()]), 
                                            array("d",[float(0.1) for xe in RESOLUTION.keys()]),
                                            array("d",[float(ye) for ye in RESOLUTION_SIGMA.values()]))

   res_part_plot = TGraphErrors(len(RESOLUTION_PART),  array("d",[float(x) for x in RESOLUTION_PART.keys()]),  
                                            array("d",[float(y) for y in RESOLUTION_PART.values()]), 
                                            array("d",[float(0.1) for xe in RESOLUTION_PART.keys()]),
                                            array("d",[float(ye) for ye in RESOLUTION_PART_SIGMA.values()]))

   lin_plot = TGraphErrors(len(LINEARITY),   array("d",[float(x) for x in LINEARITY.keys()]),  
                                      array("d",[float(y) for y in LINEARITY.values()]), 
                                      array("d",[float(0.1) for xe in LINEARITY.keys()]),
                                      array("d",[float(ye) for ye in LINEARITY_SIGMA.values()]))

    
   lin_part_plot = TGraphErrors(len(LINEARITY_PART),   array("d",[float(x) for x in LINEARITY_PART.keys()]),  
                                      array("d",[float(y) for y in LINEARITY_PART.values()]), 
                                      array("d",[float(0.1) for xe in LINEARITY_PART.keys()]),
                                      array("d",[float(ye) for ye in LINEARITY_PART_SIGMA.values()]))


   c_res = TCanvas("Resolution")

   fit_min = 0
   fit_max = 300
   fit = TF1("fit", "[0]/sqrt(x)+[1]", fit_min, fit_max)
   fit_part = TF1("fit_part", "[0]/sqrt(x)+[1]", fit_min, 1000)
   
   res_plot.SetMarkerStyle(22);
   res_plot.SetMaximum(0.08);
   res_plot.SetMinimum(0.0);
   res_plot.SetTitle("Resolution: 50um Pixels")
   res_plot.GetXaxis().SetTitle("Energy [GeV]")
   res_plot.GetYaxis().SetTitle("sigma_N / mu_N")
   res_plot.Fit(fit, "NR")
   fit.SetRange(0,1000);

   res_part_plot.SetMarkerStyle(23);
   res_part_plot.SetTitle("Resolution: Particles")
   res_part_plot.GetXaxis().SetTitle("Energy [GeV]")
   res_part_plot.GetYaxis().SetTitle("sigma_N / mu_N")
   res_part_plot.Fit(fit_part, "NR")
   fit_part.SetRange(0,1000);    

   res_part_plot.Draw("ap")
   fit_part.Draw("same")   
   res_plot.Draw("p")
   fit.Draw("same")

   pt = TPaveText(0.55,0.55,0.8,0.85, "NDC")
   pt.SetBorderSize(0)
   pt.SetFillColor(0)
   pt.AddText('Fit Range: {fit_min} - {fit_max} '.format(fit_min=fit_min, fit_max=fit_max))
   pt.AddText('#frac{{{stoch:.1%}}}{{{sqrt}}} #oplus {leakage:.1%} '.format(stoch=fit_part.GetParameter(0), sqrt="#sqrt{E}", leakage=fit_part.GetParameter(1),E="E"))
   #pt.AddText('#frac{{{noise:.1%}}}{{{E}}} #oplus #frac{{{stoch:.1%}}}{{{sqrt}}} #oplus {leakage:.1%} '.format(stoch=fit_0mm.GetParameter(0), sqrt="#sqrt{E}", leakage=fit_0mm.GetParameter(1),E="E", noise=fit_0mm.GetParameter(2)))
   pt.AddText('#frac{{{stoch:.1%}}}{{{sqrt}}} #oplus {leakage:.1%} '.format(stoch=fit.GetParameter(0), sqrt="#sqrt{E}", leakage=fit.GetParameter(1),E="E")) 
   pt.Draw()
   c_res.Print("DECal_resolution.png")

   lin_plot.SetMarkerStyle(22)
   lin_plot.SetTitle("Linearity: ")
   lin_plot.GetXaxis().SetTitle("Energy [GeV]")
   lin_plot.GetYaxis().SetTitle("Mean particles per event")
   lin_fit = TF1("lin_fit", "pol1", fit_min, fit_max)
   lin_plot.Fit(lin_fit, "NR")
   lin_fit.SetRange(0,1000)

   lin_part_plot.SetMarkerStyle(23)
   lin_part_plot.SetTitle("Linearity: ")
   lin_part_plot.GetXaxis().SetTitle("Energy [GeV]")
   lin_part_plot.GetYaxis().SetTitle("Mean particles per event")
   lin_part_fit = TF1("lin_part_fit", "pol1", fit_min, fit_max)
   lin_part_plot.Fit(lin_part_fit, "NR")
   lin_part_fit.SetRange(0,1000)   

   c_lin = TCanvas("lin_plot")
   lin_part_plot.Draw("ap")
   lin_part_fit.Draw("same")
   lin_plot.Draw("p")
   lin_fit.Draw("same")
   pt_lin = TPaveText(0.25,0.7,0.5,0.85, "NDC")
   pt_lin.SetBorderSize(0)
   pt_lin.SetFillColor(0)
   #pt_lin.AddText('Fit Range: {fit_min} - {fit_max} '.format(fit_min, fit_max))
   #pt_lin.AddText('{x2:.4g}x^2 #plus {x:.4g}x #plus {c:.4g} '.format(x2=lin_fit.GetParameter(2), x=lin_fit.GetParameter(1), c=lin_fit.GetParameter(0)))
   #pt_lin.AddText(' {x:.4g}x #plus {c:.4g} '.format(x=lin_fit.GetParameter(1), c=lin_fit.GetParameter(0)))
   pt_lin.Draw()
   c_lin.Print("DEcal_linearity.png")
   
   res_plot.Write()
   lin_plot.Write()
   fout.Write()
   fout.Close()

closeInput = raw_input("Press ENTER to exit") 
