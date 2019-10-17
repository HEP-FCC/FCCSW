# Setup ROOT
import ROOT
from ROOT import TH1F, TLegend, gPad, TCanvas, SetOwnership, TPaveText, TLine, kRed, TPad, kGray
from math import sqrt,ceil

def draw_1histogram( histo, x_axisName, y_axisName="", colour = 9, markerStyle = 21):
   prepare_histogram(histo)
   histo.SetMarkerStyle(markerStyle)
   histo.SetMarkerColor(colour)
   histo.SetLineColor(colour)
   histo.GetXaxis().SetTitle(x_axisName)
   if (y_axisName==""):
      histo.GetYaxis().SetTitle("Entries/per bin")
   else:
      histo.GetYaxis().SetTitle(y_axisName)
   maximum = 1.2*histo.GetMaximum()
   histo.SetMaximum(maximum)
   histo.Draw("ep")
   gPad.Update()
   return

def prepare_histogram(histo, title='', colour = 9, markerStyle = 21):
   if title != '':
      histo.SetTitle(title)
   histo.SetMarkerStyle(markerStyle)
   histo.SetMarkerSize(1.4)
   histo.SetMarkerColor(colour)
   histo.SetLineColor(colour)
   # set Y axis
   histo.GetYaxis().SetTitleSize(0.06)
   histo.GetYaxis().SetTitleOffset(1.1)
   histo.GetYaxis().SetLabelSize(0.045)
   histo.GetYaxis().SetNdivisions(504)
   # set X axis
   histo.GetXaxis().SetTitleSize(0.07)
   histo.GetXaxis().SetTitleOffset(1.)
   histo.GetXaxis().SetLabelSize(0.05)
   histo.GetYaxis().SetNdivisions(506)

def draw_2histograms( histo1, histo2, x_axisName, y_axisName, leg1Name, leg2Name ):
   histo1.GetXaxis().SetTitle(x_axisName)
   if (y_axisName==""):
      histo1.GetYaxis().SetTitle("Entries/per bin")
   else:
      histo1.GetYaxis().SetTitle(y_axisName)

   histo2.SetLineColor(2)

   if (leg1Name!=""):
      histo1.SetStats(0)
      histo2.SetStats(0)
      legend=TLegend(0.45,0.78,0.9,0.9)
      legend.AddEntry(histo1, leg1Name, "l")
      legend.AddEntry(histo2, leg2Name, "l")
      legend.SetTextSize(0.05)
      SetOwnership( legend, 0 )

   maximum1 = 1.2*histo1.GetMaximum()
   maximum2 = 1.2*histo2.GetMaximum()
   maximum = maximum1
   if (maximum2>maximum):
      maximum = maximum2
   histo1.SetMaximum(maximum)
   histo1.Draw()
   histo2.Draw("same")
   if (leg1Name!=""):
      legend.Draw("same")

   gPad.Update()

   return


def draw_1histogram_normalized( histo, x_axisName, y_axisName ):
   histo.Scale(1./histo.GetEntries())
   draw_1histogram(histo, x_axisName, y_axisName)
   return

def draw_2histograms_normalized( histo1, histo2, x_axisName,y_axisName, leg1Name, leg2Name ):
   histo1.Scale(1./histo1.GetEntries())
   histo2.Scale(1./histo2.GetEntries())
   draw_2histograms( histo1, histo2, x_axisName, y_axisName, leg1Name, leg2Name)
   return

def draw_hist2d(hist, titleX = "", titleY = "", title = ""):
    hist.Draw('colz')
    if titleX:
       hist.GetXaxis().SetTitle(titleX)
    if titleY:
       hist.GetYaxis().SetTitle(titleY)
    if title:
       hist.SetTitle(title)

def draw_text(lines, coordinates = [0.1,0.8,0.5,0.9], colour = 36, border = 1):
   text = TPaveText(coordinates[0],
                    coordinates[1],
                    coordinates[2],
                    coordinates[3],"brNDC")
   text.SetFillColorAlpha(0,100)
   text.SetBorderSize(border)
   text.SetTextFont(62)
   for line in lines:
      text.AddText("#color["+str(colour)+"]{"+line+"}")
      print(line)
      text.Draw()
      ROOT.SetOwnership(text,False)
   return text

def draw_rectangle(start = [0,0], end = [1,1], colour = 2, width = 2):
   lines = []
   lines.append(TLine(start[0],start[1],end[0],start[1]))
   lines.append(TLine(end[0],start[1],end[0],end[1]))
   lines.append(TLine(start[0],end[1],end[0],end[1]))
   lines.append(TLine(start[0],start[1],start[0],end[1]))
   for line in lines:
      line.SetLineColor(colour)
      line.SetLineWidth(width)
      line.Draw()
      ROOT.SetOwnership(line,False)

def prepare_graph(graph, name, title, colour = 9, markerStyle = 21, factor = 1):
   # graph settings
   graph.SetTitle(title)
   graph.SetName(name)
   graph.SetMarkerStyle(markerStyle)
   graph.SetMarkerSize(1.4)
   graph.SetMarkerColor(colour)
   graph.SetLineColor(colour)
   # set Y axis
   graph.GetYaxis().SetTitleSize(0.06)
   graph.GetYaxis().SetTitleOffset(1.1)
   graph.GetYaxis().SetLabelSize(0.045)
   graph.GetYaxis().SetNdivisions(504)
   # set X axis
   graph.GetXaxis().SetTitleSize(0.07)
   graph.GetXaxis().SetTitleOffset(1.)
   graph.GetXaxis().SetLabelSize(0.05)
   graph.GetYaxis().SetNdivisions(506)

def prepare_second_graph(secondary, main, name, title, factor = 2):
   # graph settings
   secondary.SetTitle(title)
   secondary.SetName(name)
   secondary.SetMarkerStyle(main.GetMarkerStyle())
   secondary.SetMarkerSize(main.GetMarkerSize())
   secondary.SetMarkerColor(main.GetMarkerColor())
   secondary.SetLineColor(main.GetLineColor())
   # set X axis
   secondary.GetXaxis().SetLabelSize(0)
   secondary.GetXaxis().SetTitleSize(0)
   secondary.GetXaxis().SetTickLength(main.GetXaxis().GetTickLength()*factor)
   # set Y axis
   main.GetYaxis().CenterTitle()
   secondary.GetYaxis().CenterTitle()
   secondary.GetYaxis().SetLabelSize(main.GetYaxis().GetLabelSize()*factor)
   secondary.GetYaxis().SetLabelOffset(main.GetYaxis().GetLabelOffset()/factor)
   secondary.GetYaxis().SetTitleSize(main.GetYaxis().GetTitleSize()*factor)
   secondary.GetYaxis().SetTitleOffset(main.GetYaxis().GetTitleOffset()/factor)
   secondary.GetYaxis().SetTickLength(main.GetYaxis().GetTickLength())
   secondary.GetYaxis().SetNdivisions(506)

def prepare_single_canvas(name, title):
   c = TCanvas(name, title, 1200, 900)
   c.SetTopMargin(0.01)
   c.SetRightMargin(0.03)
   c.SetLeftMargin(0.15)
   c.SetBottomMargin(0.15)
   ROOT.SetOwnership(c,False)
   return c

def prepare_double_canvas(name, title, factor = 1):
   c = TCanvas(name, title, 1200, int(900 + 900 / factor))
   pad1 = TPad("pad1","pad1",0,0,1,factor / (1. + factor))
   pad2 = TPad("pad2","pad2",0,factor / (1. + factor),1,1)
   print("heights:",factor / (1. + factor) )
   pad2.SetBottomMargin(0.01)
   pad2.SetRightMargin(0.03)
   pad2.SetLeftMargin(0.15)
   pad1.SetBorderMode(0)
   pad1.SetTopMargin(0.01)
   pad1.SetRightMargin(0.03)
   pad1.SetLeftMargin(0.15)
   pad1.SetBottomMargin(0.15)
   pad2.SetBorderMode(0)
   pad2.SetGridy()
   pad1.SetTickx(1)
   pad2.SetTickx(1)
   pad1.SetTicky(1)
   pad2.SetTicky(1)
   pad1.Draw()
   pad2.Draw()
   ROOT.SetOwnership(c,False)
   ROOT.SetOwnership(pad1,False)
   ROOT.SetOwnership(pad2,False)
   pad1.cd()
   return c, pad1, pad2

def prepare_divided_canvas(name, title, N):
   c = TCanvas(name, title, 1200, 900)
   print N
   c.Divide(int(ceil(sqrt(N))), int(ceil(N / ceil(sqrt(N)))))
   print("=====> Dividing canvas into : ",N, sqrt(N),ceil(sqrt(N)), N / ceil(sqrt(N)), ceil(N / ceil(sqrt(N))))
   print("=====> Dividing canvas into : ",ceil(sqrt(N)), ceil(N / ceil(sqrt(N))) )
   for ipad in range(1, N+1):
      pad = c.cd(ipad)
      pad.SetTopMargin(0.01)
      pad.SetRightMargin(0.03)
      pad.SetLeftMargin(0.15)
      pad.SetBottomMargin(0.15)
   ROOT.SetOwnership(c,False)
   return c
