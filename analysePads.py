import ROOT
import os

filename="./BarDECal_Readout_padMultiplicity_100GeV.txt";

ppe = ROOT.TH1F("PadsPerEvent","", 10000,0,10000);
hpp = ROOT.TH1F("HitsPerPad","", 100,0,100);

lines = [line.rstrip('\n') for line in open(filename)]
for line in lines[506:]:
  hitsPerPad = line.split(' ')
  padsPerEvent = len(hitsPerPad) - 1
  print padsPerEvent, 'pads with hits in this event'
  ppe.Fill(padsPerEvent)
  [ hpp.Fill(float(n)) for n in hitsPerPad[:-1] ]
  
import time

canvas = ROOT.TCanvas("canvas")
canvas.Divide(2,1)
canvas.cd(1)
ppe.Draw()
canvas.cd(2)
hpp.Draw()
canvas.Update()

time.sleep(30)
