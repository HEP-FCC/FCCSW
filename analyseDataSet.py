import os
import ROOT
import numpy as np
from array import array

def parseFile(fname, var):
  flist = [line.rstrip('\n') for line in open(fname)]
  parsing = False
  cache = []
  for line in flist:
    if "ApplicationMgr       INFO Application Manager Stopped successfully" in line or line=='' or "STOP" in line:
      parsing = False;
    if "unique cellIDs" in line:
      parsing = True
      continue
    if parsing:
      cache.append(line.split('\t')[var])
  return [float(i) for i in cache]

def getVarFromConfig(config=""):
  var = 0;
  if config == "Particle":
    var = 0
  if config == "Pixel":
    var = 1
  if config == "Strixel":
    var = 2
  if config == "Strixel_3":
    var = 3
  if config == "Strixel_7":
    var = 4
  if config == "Strixel_15":
    var = 5
  if config == "Strixel_31":
    var = 6
  if config == "Pad":
    var = 7
  return var

def makeLinearityPlot(det='', bfield='',config=''):

  var = getVarFromConfig(config)

  energy = []
  count = []
  
  paths = [p for p in os.listdir("./batch/"+det) if "BFIELD"+str(bfield) in p]
  for p in paths:
    fname = "./batch/"+det+"/"+p+"/logs"
    if not os.path.isfile(fname):
      continue
    e = float(p[0:p.find("GeV")])
    if e > 1.0 and e<10000:# and e<900.0 and e != 70.0 and e !=200.0:
      sqrte = 1/pow(e,0.5)
      n = parseFile(fname, var)
      mean = np.mean(n)
      
      energy.append(e)
      count.append(mean)
 
  print energy, count
  plot = ROOT.TGraph(len(energy), array("d",energy), array("d",count))
  plot.SetTitle(det+" BFIELD="+str(bfield)+" Mode:"+config)
  plot.GetXaxis().SetTitle("Energy [GeV]")
  plot.GetYaxis().SetTitle("#mu_{N}")

  return plot


def makeResolutionPlot(det='', bfield='',config=''):

  var = getVarFromConfig(config)

  energy = []
  resolution = []
  
  paths = [p for p in os.listdir("./batch/"+det) if "BFIELD"+str(bfield) in p]
  for p in paths:
    fname = "./batch/"+det+"/"+p+"/logs"
    if not os.path.isfile(fname):
      continue
    e = float(p[0:p.find("GeV")])
    if e > 1.0 and e<1001.0:# and e<900.0 and e != 70.0 and e !=200.0:
      sqrte = 1/pow(e,0.5)
      n = parseFile(fname, var)
      print "N=",len(n)
      mean = np.mean(n)
      std = np.std(n)
      res = std/mean

      energy.append(sqrte)
      resolution.append(res)
 
  print energy, resolution
  plot = ROOT.TGraph(len(energy), array("d",energy), array("d",resolution))
  plot.SetTitle(det+" BFIELD="+str(bfield)+" Mode:"+config)
  plot.SetName(det+" BFIELD="+str(bfield)+" Mode:"+config)
  plot.GetXaxis().SetTitle("1/#sqrt{E}")
  plot.GetYaxis().SetTitle("#sigma_{N}/#mu_{N}")

  return plot
      
def fitResolutionPlot(plot=None,fit="",min=0.07, max=0.35):
  fit = ROOT.TF1("fit", fit,min,max)
  plot.Fit(fit,"NR")
  fit.SetRange(0.001, 0.35)
  return fit

def fitLinearityPlot(plot=None, fit="", min=0, max=100):
  fit = ROOT.TF1("fit",fit, min, max)
  fit.FixParameter(0,0)
  plot.Fit(fit, "NR")
  fit.SetRange(0, 1000)
  return fit

def plotSingleResolution(plot=None, fit=None):
  canvas = ROOT.TCanvas(plot.GetTitle(),"single")
  canvas.cd()
  if plot is not None:
    plot.SetMarkerStyle(21)
    plot.SetMarkerSize(1)
    plot.Draw("ap")
  if fit is not None:
    fit.SetLineColor(1)
    fit.SetLineStyle(2)
    print fit.GetNpar()
    fit.DrawCopy("same")

    pt = ROOT.TPaveText(0.2,0.7,0.5,0.8,"NDC")
    pt.SetFillColor(0)
    pt.SetBorderSize(0)
    if fit.GetNpar() == 2:
      print "#frac{#sigma(E)}{E} =  #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit.GetParameter(1)*100, fit.GetParameter(0)*100)
      pt.AddText("#frac{#sigma(E)}{E} =  #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit.GetParameter(1)*100, fit.GetParameter(0)*100))
    if fit.GetNpar() == 3:
      print "#frac{#sigma(E)}{E} =  %03.1f + #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit.GetParameter(2)*100, fit.GetParameter(1)*100, fit.GetParameter(0)*100)  
      pt.AddText("#frac{#sigma(E)}{E} =  %03.1f + #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit.GetParameter(2)*100, fit.GetParameter(1)*100, fit.GetParameter(0)*100)   )
    pt.Draw()

  canvas.Update()
  return canvas

def plotDoubleResolution(plot1=None, fit1=None, plot2=None, fit2=None):
  canvas = ROOT.TCanvas("double","double")
  canvas.cd()
  if plot1 is not None:
    plot1.SetMarkerStyle(21)
    plot1.SetMarkerSize(1)
    plot1.Draw("ap")
  if fit1 is not None:
    fit1.SetLineColor(1)
    fit1.SetLineStyle(2)
    fit1.DrawCopy("same")

    pt1 = ROOT.TPaveText(0.2,0.7,0.5,0.8,"NDC")
    pt1.SetFillColor(0)
    pt1.SetBorderSize(0)
    if fit1.GetNpar() == 2:
      print "#frac{#sigma(E)}{E} =  #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit1.GetParameter(1)*100, fit1.GetParameter(0)*100)
      pt1.AddText("#frac{#sigma(E)}{E} =  #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit1.GetParameter(1)*100, fit1.GetParameter(0)*100))
    if fit1.GetNpar() == 3:
      print "#frac{#sigma(E)}{E} =  %03.1f + #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit1.GetParameter(2)*100, fit1.GetParameter(1)*100, fit1.GetParameter(0)*100)  
      pt1.AddText("#frac{#sigma(E)}{E} =  %03.1f + #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit1.GetParameter(2)*100, fit1.GetParameter(1)*100, fit1.GetParameter(0)*100)   )
    pt1.Draw()

  if plot2 is not None:
    plot2.SetMarkerStyle(22)
    plot2.SetMarkerSize(1)
    plot2.Draw("p")
  if fit2 is not None:
    fit2.SetLineColor(1)
    fit2.SetLineStyle(3)
    fit2.DrawCopy("same")

    pt2 = ROOT.TPaveText(0.2,0.6,0.5,0.7,"NDC")
    pt2.SetFillColor(0)
    pt2.SetBorderSize(0)
    if fit2.GetNpar() == 2:
      print "#frac{#sigma(E)}{E} =  #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit2.GetParameter(1)*100, fit2.GetParameter(0)*100)
      pt2.AddText("#frac{#sigma(E)}{E} =  #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit2.GetParameter(1)*100, fit2.GetParameter(0)*100))
    if fit2.GetNpar() == 3:
      print "#frac{#sigma(E)}{E} =  %03.1f + #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit2.GetParameter(2)*100, fit2.GetParameter(1)*100, fit2.GetParameter(0)*100)  
      pt2.AddText("#frac{#sigma(E)}{E} =  %03.1f + #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit2.GetParameter(2)*100, fit2.GetParameter(1)*100, fit2.GetParameter(0)*100)   )
    pt2.Draw()

  canvas.Update()
  return canvas 

def plotResolution(plot=None, fit=None, cname=None):

  if plot is None:
    print "Error: No plots were sent for plotting"
    return
  if len is None:
    print "Warning: no fits were sent for plotting"
  if plot is not None and fit is not None and len(plot) != len(fit) :
    print "Error: length of plot and fit are not the same!!!"
    return  

  if cname is None:
    cname = "rC_"+plot[0].GetTitle()+"_"+str(len(plot))+"plots"
  rC = ROOT.TCanvas(cname,cname)
  rC.cd()

#  if fit is not None:
#    legend = ROOT.TLegend(0.6,(0.45-len(plot)*0.08), 0.85, 0.45)
#  else:
 #   legend = ROOT.TLegend(0.6,(0.45-len(plot)*0.04), 0.85, 0.45)

  if fit is not None:
    legend = ROOT.TLegend(0.125,(0.85-len(plot)*0.08), 0.525, 0.875)
  else:
    legend = ROOT.TLegend(0.125,(0.85-len(plot)*0.04), 0.525, 0.875)

  #legend.SetFillColor(0)
  legend.SetFillStyle(0)
  legend.SetBorderSize(0)

  maxy = 0
  miny = 1000
  
  for i in np.arange(0,len(plot)):
    print "\n",plot[i].GetTitle()
    print "--------------------"
    # first draw the plots
    if plot[i] is not None:
      plot[i].SetMarkerStyle(int(21+i))
      plot[i].SetMarkerSize(0.5)
      #plot[i].SetMaximum(0.05)
      
      max = ROOT.TMath.MaxElement(plot[i].GetN(),plot[i].GetY());
      if max > maxy:
        maxy = max
      min = ROOT.TMath.MinElement(plot[i].GetN(),plot[i].GetY());
      if min < miny:
        miny = min

      if i == 0:
        plot[i].Draw("ap")
      else: 
        plot[i].Draw("p")
      legend.AddEntry(plot[i], plot[i].GetName(), "p")
    # then draw the fit to the plot
    if fit is not None and fit[i] is not None:
      fit[i].SetLineStyle(int(2+i))
      fit[i].Draw("same")
      fit_res = ""
      if fit[i].GetNpar() == 2:
        fit_res="#frac{#sigma(E)}{E} =  #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit[i].GetParameter(1)*100, fit[i].GetParameter(0)*100)
      elif fit[i].GetNpar() == 3:
        fit_res="#frac{#sigma(E)}{E} =  %03.1f + #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit[i].GetParameter(2)*100, fit[i].GetParameter(1)*100, fit[i].GetParameter(0)*100)  
      print fit_res
      legend.AddEntry(fit[i], fit_res, "l")
  
  plot[0].SetMaximum(maxy*1.1)
  plot[0].SetMinimum(miny*0.85) 
  legend.Draw()   
  rC.Update()
  rC.Print(rC.GetTitle()+".png")   

def plotLinearity(plot=None, fit=None, cname=None):

  if plot is None:
    print "Error: No plots were sent for plotting"
    return
  if len is None:
    print "Warning: no fits were sent for plotting"
  if plot is not None and fit is not None and len(plot) != len(fit) :
    print "Error: length of plot and fit are not the same!!!"
    return  

  if cname is None:
    cname = "lC_"+plot[0].GetTitle()+"_"+str(len(plot))+"plots"
  lC = ROOT.TCanvas(cname,cname)
  lC.cd()

  if fit is not None:
    legend = ROOT.TLegend(0.125,(0.85-len(plot)*0.08), 0.525, 0.875)
  else:
    legend = ROOT.TLegend(0.125,(0.85-len(plot)*0.04), 0.525, 0.875)

  legend.SetFillStyle(0)
  legend.SetBorderSize(0)

  miny = 1000
  maxy = 0

  for i in np.arange(0,len(plot)):
    print "\n",plot[i].GetTitle()
    print "--------------------"
    # first draw the plots
    if plot[i] is not None:
      plot[i].SetMarkerStyle(int(21+i))
      plot[i].SetMarkerSize(0.5)
      #plot[i].GetXaxis().SetRangeUser(0,100)
     
      max = ROOT.TMath.MaxElement(plot[i].GetN(),plot[i].GetY());
      if max > maxy:
        maxy = max
      min = ROOT.TMath.MinElement(plot[i].GetN(),plot[i].GetY());
      if min < miny:
        miny = min

      if i == 0:
        plot[i].Draw("ap")
      else:
        plot[i].Draw("p")
      legend.AddEntry(plot[i], plot[i].GetTitle(), "p")
    # then draw the fit to the plot
    if fit is not None and fit[i] is not None:
      fit[i].SetLineStyle(int(2+i))
      if i==0:
        fit[i].Draw("same")
      fit_res = ""
      if fit[i].GetNpar() == 2:
        fit_res="N =  %03.1fE + %03.1f" % (fit[i].GetParameter(1), fit[i].GetParameter(0))
      elif fit[i].GetNpar() == 3:
        fit_res="#frac{#sigma(E)}{E} =  %03.1f + #frac{%03.1f}{#sqrt{E}} + %03.1f" % (fit[i].GetParameter(2)*100, fit[i].GetParameter(1)*100, fit[i].GetParameter(0)*100)  
      print fit_res
      #legend.AddEntry(fit[i], fit_res, "l")
   
  legend.Draw()   

  plot[0].SetMinimum(miny*0.85)
  plot[0].SetMaximum(maxy*1.15)
  lC.Update()
  lC.Print(lC.GetTitle()+".png")

def analyseSingleDataSet(detector="", b=1, name=None):

  res_i = makeResolutionPlot(det=detector, bfield=b, config="Particle")
  #res_i.SetNameTitle("Particle","Particle")
  res_i.SetTitle(detector+" BField="+str(b))
  res_i.SetName("Particle")
  res_p = makeResolutionPlot(det=detector, bfield=b, config="Pixel")
  res_p.SetNameTitle("Pixel","Pixel")
  res_s = makeResolutionPlot(det=detector, bfield=b, config="Strixel")
  res_s.SetNameTitle("Strixel","Strixel")
  res_s3 = makeResolutionPlot(det=detector, bfield=b, config="Strixel_3")
  res_s3.SetNameTitle("Strixel_3","Strixel_3")
  res_s7 = makeResolutionPlot(det=detector, bfield=b, config="Strixel_7")
  res_s7.SetNameTitle("Strixel_7","Strixel_7")
  res_s15 = makeResolutionPlot(det=detector, bfield=b, config="Strixel_15")
  res_s15.SetNameTitle("Strixel_15","Strixel_15")
  res_s31 = makeResolutionPlot(det=detector, bfield=b, config="Strixel_31")
  res_s31.SetNameTitle("Strixel_31","Strixel_31")
  res_canvas = plotResolution([res_i,res_p, res_s, res_s3, res_s7, res_s15, res_s31],cname="res_"+name)


  lin_i = makeLinearityPlot(det=detector, bfield=b, config="Particle")
  lin_p = makeLinearityPlot(det=detector, bfield=b, config="Pixel")
  lin_s = makeLinearityPlot(det=detector, bfield=b, config="Strixel")
  lin_s3 = makeLinearityPlot(det=detector, bfield=b, config="Strixel_3")
  lin_s7 = makeLinearityPlot(det=detector, bfield=b, config="Strixel_7")
  lin_s15 = makeLinearityPlot(det=detector, bfield=b, config="Strixel_15")
  lin_s31 = makeLinearityPlot(det=detector, bfield=b, config="Strixel_31")
  lin_canvas = plotLinearity([lin_i,lin_p, lin_s, lin_s3, lin_s7, lin_s15, lin_s31],cname="lin_"+name)

detector="30Layers_3.5mmW_50umPixels"
bfield_on = 1

analyseSingleDataSet(detector, bfield_on, "test_plot")

"""
#
# look at the particle configurations... should be the same
#
#

res_25_p_b0 = makeResolutionPlot(det="50Layers_2.1mmW_25umPixels", bfield=0, config="Particle")
res_25_p_b1 = makeResolutionPlot(det="50Layers_2.1mmW_25umPixels", bfield=1, config="Particle")
res_50_p_b0 = makeResolutionPlot(det="50Layers_2.1mmW", bfield=0, config="Particle")
res_50_p_b1 = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Particle")

res_25_p_b0_fit = fitResolutionPlot(res_25_p_b0, "pol1",0.12,0.35)
res_25_p_b1_fit = fitResolutionPlot(res_25_p_b1, "pol1",0.12,0.35)
res_50_p_b0_fit = fitResolutionPlot(res_50_p_b0, "pol1",0.12,0.35)
res_50_p_b1_fit = fitResolutionPlot(res_50_p_b1, "pol1",0.12,0.35)
plotResolution([res_25_p_b1,res_50_p_b0, res_50_p_b1],[res_25_p_b1_fit,res_50_p_b0_fit,res_50_p_b1_fit])

lin_25_p_b0 = makeLinearityPlot(det="50Layers_2.1mmW_25umPixels", bfield=0, config="Particle")
lin_25_p_b1 = makeLinearityPlot(det="50Layers_2.1mmW_25umPixels", bfield=1, config="Particle")
lin_50_p_b0 = makeLinearityPlot(det="50Layers_2.1mmW", bfield=0, config="Particle")
lin_50_p_b1 = makeLinearityPlot(det="50Layers_2.1mmW", bfield=1, config="Particle")

lin_25_p_b0_fit = fitLinearityPlot(lin_25_p_b0, "pol1", 0,100)
lin_25_p_b1_fit = fitLinearityPlot(lin_25_p_b1, "pol1", 0,100)
lin_50_p_b0_fit = fitLinearityPlot(lin_50_p_b0, "pol1", 0,100)
lin_50_p_b1_fit = fitLinearityPlot(lin_50_p_b1, "pol1", 0,100)

plotLinearity([lin_25_p_b0, lin_25_p_b1, lin_50_p_b0, lin_50_p_b1], [lin_25_p_b0_fit,lin_25_p_b1_fit, lin_50_p_b0_fit, lin_50_p_b1_fit])

#
# compare bfield=1 for different pixel size
#

res_25_p_b1 = makeResolutionPlot(det="50Layers_2.1mmW_25umPixels", bfield=1, config="Pixel")
res_50_p_b1 = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Pixel")

res_25_p_b1_fit = fitResolutionPlot(res_25_p_b1, "pol1",0.12,0.35)
res_50_p_b1_fit = fitResolutionPlot(res_50_p_b1, "pol1",0.12,0.35)
plotResolution([res_25_p_b1, res_50_p_b1],[res_25_p_b1_fit,res_50_p_b1_fit])

lin_25_p_b0 = makeLinearityPlot(det="50Layers_2.1mmW_25umPixels", bfield=0, config="Pixel")
lin_25_p_b1 = makeLinearityPlot(det="50Layers_2.1mmW_25umPixels", bfield=1, config="Pixel")
lin_50_p_b0 = makeLinearityPlot(det="50Layers_2.1mmW", bfield=0, config="Pixel")
lin_50_p_b1 = makeLinearityPlot(det="50Layers_2.1mmW", bfield=1, config="Pixel")

lin_25_p_b0_fit = fitLinearityPlot(lin_25_p_b0, "pol1", 0,100)
lin_25_p_b1_fit = fitLinearityPlot(lin_25_p_b1, "pol1", 0,100)
lin_50_p_b0_fit = fitLinearityPlot(lin_50_p_b0, "pol1", 0,100)
lin_50_p_b1_fit = fitLinearityPlot(lin_50_p_b1, "pol1", 0,100)

plotLinearity([lin_25_p_b0, lin_25_p_b1, lin_50_p_b0, lin_50_p_b1], [lin_25_p_b0_fit,lin_25_p_b1_fit, lin_50_p_b0_fit, lin_50_p_b1_fit])
"""

"""
res_50_pa_b1 = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Particle")
res_50_p_b1 = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Pixel")
res_50_s3_b1 = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Strixel_3")
res_50_s7_b1 = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Strixel_7")
res_50_s15_b1 = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Strixel_15")
res_50_s31_b1 = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Strixel_31")

res_50_pa_b1_fit = fitResolutionPlot(res_50_pa_b1, "pol1",0.12,0.35)
res_50_p_b1_fit = fitResolutionPlot(res_50_p_b1, "pol1",0.12,0.35)
res_50_s3_b1_fit = fitResolutionPlot(res_50_s3_b1, "pol1",0.17,0.35)
res_50_s7_b1_fit = fitResolutionPlot(res_50_s7_b1, "pol1",0.12,0.35)

plotResolution([res_50_pa_b1,res_50_p_b1,res_50_s3_b1,res_50_s7_b1, res_50_s15_b1],cname="spec_meeting_resolution")#,[res_50_pa_b1_fit,res_50_p_b1_fit,res_50_s3_b1_fit,res_50_s7_b1_fit])
plotResolution([res_50_pa_b1,res_50_s15_b1, res_50_s31_b1],cname="spec_meeting_resolution_highE")

lin_50_pa_b1 = makeLinearityPlot(det="50Layers_2.1mmW", bfield=1, config="Particle")
lin_50_p_b1 = makeLinearityPlot(det="50Layers_2.1mmW", bfield=1, config="Pixel")
lin_50_s_b1 = makeLinearityPlot(det="50Layers_2.1mmW", bfield=1, config="Strixel")
lin_50_s3_b1 = makeLinearityPlot(det="50Layers_2.1mmW", bfield=1, config="Strixel_3")
lin_50_s7_b1 = makeLinearityPlot(det="50Layers_2.1mmW", bfield=1, config="Strixel_7")

lin_50_pa_b1_fit = fitLinearityPlot(lin_50_pa_b1, "pol2",0,100)
lin_50_p_b1_fit = fitLinearityPlot(lin_50_p_b1, "pol2",0,100)
lin_50_s_b1_fit = fitLinearityPlot(lin_50_s_b1, "pol2",0,100)
lin_50_s3_b1_fit = fitLinearityPlot(lin_50_s3_b1, "pol2",0,100)
lin_50_s7_b1_fit = fitLinearityPlot(lin_50_s7_b1, "pol2",0,100)

plotLinearity([lin_50_pa_b1,lin_50_p_b1,lin_50_s_b1, lin_50_s3_b1,lin_50_s7_b1]  ,[lin_50_pa_b1_fit,lin_50_p_b1_fit,lin_50_s_b1_fit,lin_50_s3_b1_fit,lin_50_s7_b1_fit], cname="spec_meeting_linearity")
"""
"""
#
# compare the pixel size
#

res_50_s7_b1 = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Strixel_7")
res_60_s7_b1 = makeResolutionPlot(det="50Layers_2.1mmW_60umPixels", bfield=1, config="Strixel_7")
res_50_p_b1 = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Strixel_15")
res_60_p_b1 = makeResolutionPlot(det="50Layers_2.1mmW_60umPixels", bfield=1, config="Strixel_15")
res_50_p_b1_fit = fitResolutionPlot(res_50_p_b1, "pol1",0.12,0.35)
res_60_p_b1_fit = fitResolutionPlot(res_60_p_b1, "pol1",0.12,0.35)
#plotResolution([res_50_pa_b1, res_50_s7_b1,res_60_s7_b1,res_50_p_b1,res_60_p_b1],cname="test")#,[res_50_p_b1_fit,res_60_p_b1_fit],cname="test")
plotResolution([res_60_p_b1,res_50_s7_b1],cname="test2")

lin_50_s7_b1 = makeLinearityPlot(det="50Layers_2.1mmW", bfield=1, config="Strixel_7")
lin_60_s7_b1 = makeLinearityPlot(det="50Layers_2.1mmW_60umPixels", bfield=1, config="Strixel_7")
lin_50_p_b1 = makeLinearityPlot(det="50Layers_2.1mmW", bfield=1, config="Strixel_15")
lin_60_p_b1 = makeLinearityPlot(det="50Layers_2.1mmW_60umPixels", bfield=1, config="Strixel_15")
lin_50_p_b1_fit = fitLinearityPlot(res_50_p_b1, "pol1",0,100)
lin_60_p_b1_fit = fitLinearityPlot(res_60_p_b1, "pol1",0,100)
plotLinearity([lin_50_s7_b1,lin_60_s7_b1,lin_50_p_b1,lin_60_p_b1], cname="test_lin")#,[lin_50_p_b1_fit,lin_60_p_b1_fit],cname="test_lin")
"""



res_50L = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Pixel")
res_50L_fit = fitResolutionPlot(res_50L, "pol1", 0.1, 0.35)

lin_50L = makeLinearityPlot(det="50Layers_2.1mmW", bfield=1, config="Pixel")
lin_50L_truth = makeLinearityPlot(det="50Layers_2.1mmW", bfield=1, config="Particle")
plotLinearity([lin_50L_truth, lin_50L])
"""
res_35L = makeResolutionPlot(det="30Layers_3.5mmW_50umPixels", bfield=1, config="Pixel")
res_35L_fit = fitResolutionPlot(res_35L, "pol1", 0.1, 0.35)
plotResolution([res_50L,res_35L],[res_50L_fit,res_35L_fit], cname="CompareLayers")

#
# compare the epi thickness
#
res_25um = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Pixel")
res_25um_fit = fitResolutionPlot(res_25um, "pol1", 0.1, 0.35)
res_15um = makeResolutionPlot(det="50Layers_2.1mmW_50umPixels_15umEpi", bfield=1, config="Pixel")
res_15um_fit = fitResolutionPlot(res_15um, "pol1", 0.1, 0.35)
plotResolution([res_25um,res_15um],[res_25um_fit,res_15um_fit], cname="CompareEpi")

#
# compare the pizel size
#
res_25umPixels = makeResolutionPlot(det="50Layers_2.1mmW_25umPixels", bfield=1, config="Pixel")
res_25umPixels_fit = fitResolutionPlot(res_25umPixels, "pol1", 0.1,0.35)
res_50umPixels = makeResolutionPlot(det="50Layers_2.1mmW", bfield=1, config="Pixel")
res_50umPixels_fit = fitResolutionPlot(res_50umPixels, "pol1", 0.1,0.35)
res_60umPixels = makeResolutionPlot(det="50Layers_2.1mmW_60umPixels", bfield=1, config="Pixel")
res_60umPixels_fit = fitResolutionPlot(res_60umPixels, "pol1", 0.1,0.35)
res_100umPixels = makeResolutionPlot(det="50Layers_2.1mmW_100umPixels", bfield=1, config="Pixel")
res_100umPixels_fit = fitResolutionPlot(res_100umPixels, "pol1", 0.1,0.35)
plotResolution(cname="PixelPitch", plot=[res_25umPixels,res_50umPixels,res_60umPixels,res_100umPixels], fit=[res_25umPixels_fit,res_50umPixels_fit,res_60umPixels_fit,res_100umPixels_fit])
#
# look at effect of mask
#

res_mask = makeResolutionPlot(det="50Layers_2.1mmW_50umPixels_Mask", bfield=1, config="Pixel")
res_mask_fit = fitResolutionPlot(res_mask, "pol1", 0.1,0.35)
plotResolution([res_50L, res_mask],[res_50L_fit, res_mask_fit], cname="CompareMask")
"""
import time 
time.sleep(200)
