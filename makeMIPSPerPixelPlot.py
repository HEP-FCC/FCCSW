import ROOT
import os
import numpy as np
import time

config="30Layers_3.5mmW"
dir="./batch/"+config+"/"
file="BarDECal_Readout_mipsPerPixel.txt"
energies = os.listdir(dir)

energy = np.zeros(len(energies))
mean = np.zeros(len(energies))
single = np.zeros(len(energies))

for i in np.arange(len(energies)):
  ene = energies[i]

  path=dir+"/"+ene+"/"+file
  print path

  if not os.path.exists(path):
    continue

  #h = ROOT.TH1F("counts", 
  N = [float(n) for n in open(path,'r').read().split(' ')[:-1] if "L" not in n]
  print len(N)  

  histo = ROOT.TH1F(ene+"GeV mipsPerPixel","mips per pixel", 20, -0.5, 19.5)
  for n in N:
    histo.Fill(n)

  if "100GeV" in ene:
    histo.Draw()
    time.sleep(10)

  e = float(path[len(dir)+1:path.find("GeV")])
  if histo.GetEntries() > 0:
    m = histo.GetMean()
    s = histo.GetBinContent(2) / histo.GetEntries()
  print s


