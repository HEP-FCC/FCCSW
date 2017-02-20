"""
____________________________________________________
    FCC style plots (inspired by CMS TDR-Style)
____________________________________________________
"""
from ROOT import gROOT, gStyle, kWhite, kGray
import ROOT

class FCCStyle():
    fillcolors = [ROOT.kGray, ROOT.kRed, ROOT.kOrange, ROOT.kSpring+10, ROOT.kCyan-10, ROOT.kAzure-9, ROOT.kMagenta-9]
    linecolors = [ROOT.kBlack, ROOT.kRed+1, ROOT.kGreen+3, ROOT.kCyan+1, ROOT.kAzure-3, ROOT.kMagenta+2, ROOT.kGray+2]
    def __init__(self):
	      pass


    @staticmethod
    def initialize(fitresults=True, grid=False):
        gROOT.SetStyle("Plain")
        gStyle.SetOptFit()
        gStyle.SetOptStat(0)
        # For the canvas:
        gStyle.SetCanvasBorderMode(0)
        gStyle.SetCanvasColor(kWhite)
        gStyle.SetCanvasDefH(600) #Height of canvas
        gStyle.SetCanvasDefW(600) #Width of canvas
        gStyle.SetCanvasDefX(0)   #POsition on screen
        gStyle.SetCanvasDefY(0)

        # For the Pad:
        gStyle.SetPadBorderMode(0)
        gStyle.SetPadColor(kWhite)
        gStyle.SetPadGridX(False)
        gStyle.SetPadGridY(False)
        gStyle.SetGridColor(0)
        gStyle.SetGridStyle(3)
        gStyle.SetGridWidth(1)

        # For the frame:
        gStyle.SetFrameBorderMode(0)
        gStyle.SetFrameBorderSize(1)
        gStyle.SetFrameFillColor(kWhite)
        gStyle.SetFrameFillStyle(1000)
        gStyle.SetFrameLineColor(1)
        gStyle.SetFrameLineStyle(1)
        gStyle.SetFrameLineWidth(1)

        # For the histo:
        gStyle.SetHistLineColor(1)
        gStyle.SetHistLineStyle(0)
        gStyle.SetHistLineWidth(2)

        gStyle.SetEndErrorSize(2)
        gStyle.SetErrorX(0.)

        gStyle.SetMarkerStyle(20)

        #For the fit/function:
        gStyle.SetOptFit(1)
        gStyle.SetFitFormat("5.4g")
        gStyle.SetFuncColor(2)
        gStyle.SetFuncStyle(1)
        gStyle.SetFuncWidth(1)

        #For the date:
        gStyle.SetOptDate(0)

        # For the statistics box:
        gStyle.SetOptFile(0)
        gStyle.SetOptStat(0) # To display the mean and RMS:   SetOptStat("mr")
        gStyle.SetStatColor(kWhite)
        gStyle.SetStatFont(42)
        gStyle.SetStatFontSize(0.025)
        gStyle.SetStatTextColor(1)
        gStyle.SetStatFormat("6.4g")
        gStyle.SetStatBorderSize(1)
        gStyle.SetStatH(0.1)
        gStyle.SetStatW(0.15)

        # Margins:
        gStyle.SetPadTopMargin(0.05)
        gStyle.SetPadLeftMargin(0.16)
        gStyle.SetPadRightMargin(0.04) # top group adaption, original is 0.02
        gStyle.SetPadBottomMargin(0.13)

        # For the Global title:

        gStyle.SetOptTitle(0)
        gStyle.SetTitleFont(42)
        gStyle.SetTitleColor(1)
        gStyle.SetTitleTextColor(1)
        gStyle.SetTitleFillColor(10)
        gStyle.SetTitleFontSize(0.05)

        # For the axis titles:
        gStyle.SetTitleColor(1, "XYZ")
        gStyle.SetTitleFont(42, "XYZ")
        gStyle.SetTitleSize(0.06, "XYZ")
        gStyle.SetTitleXOffset(0.9)
        gStyle.SetTitleYOffset(1.25)

        # For the axis labels:

        gStyle.SetLabelColor(1, "XYZ")
        gStyle.SetLabelFont(42, "XYZ")
        gStyle.SetLabelOffset(0.007, "XYZ")
        gStyle.SetLabelSize(0.05, "XYZ")
        #gStyle.SetLabelSize(0.04, "XYZ")

        # For the axis:

        gStyle.SetAxisColor(1, "XYZ")
        gStyle.SetStripDecimals(True)
        gStyle.SetTickLength(0.03, "XYZ")
        gStyle.SetNdivisions(510, "XYZ")
        gStyle.SetPadTickX(1)  # To get tick marks on the opposite side of the frame
        gStyle.SetPadTickY(1)

        # Change for log plots:
        gStyle.SetOptLogx(0)
        gStyle.SetOptLogy(0)
        gStyle.SetOptLogz(0)

        gStyle.SetPalette(1) #(1,0)

        # another top group addition
        gStyle.SetHatchesSpacing(1.0)

        # Postscript options:
        gStyle.SetPaperSize(20., 20.)
        # For graphs
        gStyle.SetErrorX(0)  # suppress error along x

        if grid:
          gStyle.SetPadGridX(gridOn)
          gStyle.SetPadGridY(gridOn)
          gStyle.SetGridColor(kGray)
