from ROOT import gSystem
from ROOT import TCanvas, TFile, TGraph,TF1, gPad, CaloAnalysis_digital, TH3F
import os

PARTICLE = "30Layers_3.5mmW_5mmAir_50umPixels"
ENERGY = [10,20,30,40,50,60,70,80,90,100,200,300,400,500,600,700,800,900,1000]
RESOLUTION = {}
LINEARITY = {}

fin = TFile(PARTICLE+".root")


