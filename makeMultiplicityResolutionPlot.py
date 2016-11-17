import ROOT
import os
import numpy as np

config="30Layers_3.5mmW"
dir="./batch/"+config+"/"
file="BarDECal_Readout_pixelsOverThreshold.txt"
energies = os.listdir(dir)

energy = np.zeros(len(energies))
mean = np.zeros(len(energies))
sigma = np.zeros(len(energies))
resolution = np.zeros(len(energies))

for i in np.arange(len(energies)):
  ene = energies[i]

  path=dir+"/"+ene+"/"+file
  print path

  if not os.path.exists(path):
    continue

  #h = ROOT.TH1F("counts", 
  N = [float(n) for n in open(path,'r').read().split(' ')[:-1]]

  print len(N)  

  e = float(path[len(dir)+1:path.find("GeV")])
  m = np.mean(N)
  s = np.std(N)
  r = s/m

  energy[i] = e
  mean[i] = m
  sigma[i] = s
  resolution[i] = r
  print e, m, s, r

linearity_canvas = ROOT.TCanvas("linearity_canvas")
linearity_canvas.cd()
linearity = ROOT.TGraph(len(energy),energy,mean)
linearity.SetTitle("nPixelsOver Threshold / Event as function of single e- energy")
linearity.GetXaxis().SetTitle("Energy [GeV]")
linearity.GetYaxis().SetTitle("nPixelsOverThreshold / Event")
linearity_fit = ROOT.TF1("linearity_fit", "pol1", 0,400)
linearity_fit.FixParameter(0,0)
linearity.Fit(linearity_fit, "NR")
linearity.Draw("A*")
linearity_fit.SetRange(0,1000)
linearity_fit.Draw("same")
label = ROOT.TPaveText(0.3,0.7,0.5,0.8, "NDC")
label.SetFillColor(0)
label.SetBorderSize(0)
label.AddText(config)
label.Draw()
linearity_canvas.Print(config+"_linearity.png")

energy_resolution_canvas = ROOT.TCanvas("energy_resolution_canvas")
energy_resolution_canvas.cd()
indices = np.logical_and(energy>0, energy<501)
energy_resolution = ROOT.TGraph(len(energy[indices]), 1/np.sqrt(energy[indices]), resolution[indices])
energy_resolution.SetTitle("Energy resolution")
energy_resolution.GetXaxis().SetTitle("1/#sqrt{Energy [GeV]}")
energy_resolution.GetYaxis().SetTitle("#sigma_{N}/N")
energy_resolution.Draw("A*")
energy_resolution_fit = ROOT.TF1("energy_resolution_fit", "pol1", 0.04,1.0)
energy_resolution.Fit(energy_resolution_fit, "NR")
energy_resolution_fit.Draw("same")
energy_resolution_info = ROOT.TPaveText(0.2,0.6,0.5,0.8,"NDC")
energy_resolution_info.SetBorderSize(0)
#energy_resolution_info.SetTextFont(energy_resolution.GetTitleFontSize())
#energy_resolution_info.SetTextSize(energy_resolution.GetTextSize())
energy_resolution_info.SetFillColor(0)
energy_resolution_info.AddText(config)
energy_resolution_info.AddText("25um epi")
energy_resolution_info.AddText("threshold = 350e-")
energy_resolution_info.AddText(str(round(energy_resolution_fit.GetParameter(1)*100,1))+"%/#sqrt{E}")
energy_resolution_info.Draw()
energy_resolution_canvas.Print(config+"_resolution.png")


import time
time.sleep(600)


