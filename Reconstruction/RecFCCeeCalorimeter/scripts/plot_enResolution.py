import calo_init
calo_init.add_defaults()
calo_init.parser.add_argument("-t","--title", default="Energy resolution", help="Graph title", type=str)
calo_init.parser.add_argument("-n","--histogramName", default="energy", help="Name of the histogram containing the energy distribution", type = str)
calo_init.parser.add_argument("-m","--axisMax", help="Maximum of the axis", type = float)
calo_init.parser.add_argument("--noLinearity", help="If linearity plot should not be drawn", action = 'store_true')
calo_init.parser.add_argument("--roundBrackets", help="Use round brackets for unit", action = 'store_true')
calo_init.parser.add_argument("--technical", help="Print parameters with all details", action = 'store_true')
calo_init.parser.add_argument("--specialLabel", help="Additional label to be plotted", type=str)
calo_init.parse_args()
calo_init.print_config()

histName = calo_init.args.histogramName

print("Draw linearity: ", not calo_init.args.noLinearity)

from ROOT import gSystem, gROOT, TCanvas, TGraphErrors, TF1, gStyle, kRed, kBlue, kGray, TFile, TTree, TPad, TH1F, TChain
from draw_functions import prepare_graph, prepare_second_graph, prepare_single_canvas, prepare_double_canvas, draw_text
import numpy
from math import sqrt

gRes = TGraphErrors()
gLin = TGraphErrors()

# first get all the resolutions and prepare graphs
i = 0

f = TChain("events")
f.Add("output_allCalo_reco_noise_new3*.root")
f.Add("output_15GeV_Noise.root")
f.Add("output_5GeV_Noise.root")
f.Add("output_50GeV_Noise.root")
dtree = f
for energy, filename in [
        (20,"output_energy20Gev.root"), (10,"output_energy10Gev.root"), 
        (15, "output_energy5Gev.root"), 
        (5, "output_energy5Gev.root"), 
        (50, "output_energy5Gev.root"), 
        ]:
    #f = TFile(filename, "READ")
    ifile = i
    i = i+1
    htotal= TH1F("energy", "Energy of clusters (e^{-}, "+str(energy)+" GeV);energy (GeV);fraction of events", 99, 0.8*energy,1.2*energy)
    dtree.Draw("CaloClusters.core.energy>>energy", " sqrt(GenParticles.core.p4.px*GenParticles.core.p4.px + GenParticles.core.p4.py*GenParticles.core.p4.py + GenParticles.core.p4.pz*GenParticles.core.p4.pz) < " + str(energy + 0.1) + " && sqrt(GenParticles.core.p4.px*GenParticles.core.p4.px + GenParticles.core.p4.py*GenParticles.core.p4.p    y + GenParticles.core.p4.pz*GenParticles.core.p4.pz) > " + str(energy -0.1))
    C = TCanvas()
    htotal.Draw()
    C.Print("h"+str(energy)+".png")
    C.Print("h"+str(energy)+".pdf")
    #raw_input()
    #htotal = f.Get(histName)
    myfunPre = TF1("firstGaus","gaus", htotal.GetMean() - 2. * htotal.GetRMS(),
                   htotal.GetMean() + 2. * htotal.GetRMS())
    resultPre = htotal.Fit(myfunPre, "SRQN")
    myfun = TF1("finalGaus", "gaus", resultPre.Get().Parameter(1) - 2. * resultPre.Get().Parameter(2),
                resultPre.Get().Parameter(1) + 2. * resultPre.Get().Parameter(2) )
    result = htotal.Fit(myfun, "SRQN")
    resolution = result.Get().Parameter(2) / result.Get().Parameter(1)
    resolutionErrorSigma = result.Get().Error(2) / result.Get().Parameter(1)
    resolutionErrorMean = result.Get().Error(1) * result.Get().Parameter(2) / ( result.Get().Parameter(1) ** 2)
    resolutionError = sqrt( resolutionErrorSigma ** 2 +  resolutionErrorMean ** 2 )
    linearity = ( result.Get().Parameter(1) - energy ) / energy
    linearityError = result.Get().Error(1) / energy
    gRes.SetPoint(ifile, energy, resolution)
    gRes.SetPointError(ifile, 0, resolutionError)
    gLin.SetPoint(ifile, energy, linearity)
    gLin.SetPointError(ifile, 0, linearityError)

# Set properties of the graph
if filename.find("Bfield0") > 0:
    colour = kRed + 1 # red colour if no B field in the file name
else:
    colour = kBlue + 1 # blue otherwise (default)

if calo_init.args.roundBrackets:
    prepare_graph(gRes, "resolution", ";E_{beam} (GeV);#sigma_{E_{rec}}/#LTE_{rec}#GT", colour, 21)
else:
    prepare_graph(gRes, "resolution", ";E_{beam} [GeV];#sigma_{E_{rec}}/#LTE_{rec}#GT", colour, 21)
factor=2 # meaning the upper plot is twice smaller than the bottom plot
prepare_second_graph(gLin, gRes, "linearity", ";E_{beam} [GeV];(#LTE_{rec}#GT-E_{beam})/E_{beam}", factor)
gLin.GetYaxis().SetRangeUser(-0.9,0.9)

# Prepare canvas
if not calo_init.args.noLinearity:
    cRes, padRes, padLin = prepare_double_canvas("resolution","Energy resolution", factor)
    padRes.cd()
else:
    cRes = prepare_single_canvas("resolution","Energy resolution")
    cRes.cd()

# Fit energy resolution
fRes = TF1("res", "sqrt([0]*[0] + pow([1]/sqrt(x),2) + pow([2]/x,2) )",5,600)
fRes.SetParName(0,"const")
fRes.SetParName(1,"sqrt")
fRes.SetParName(2,"lin")
fRes.SetLineColor(colour)
fRes.SetParLimits(0,0,0.0026507)
fitResult = gRes.Fit(fRes, 'S')

# Draw graph and all labels
gRes.Draw("ape")
if calo_init.args.axisMax:
    gRes.GetYaxis().SetRangeUser(0, calo_init.args.axisMax)
formula = "#frac{#sigma_{E}}{E} = " + str(round(fitResult.Get().Parameter(0)*100,2))+"% #oplus #frac{"+str(round(fitResult.Get().Parameter(2),2)) + "GeV}{E} #oplus #frac{"+str(round(fitResult.Get().Parameter(1)*100,2))+"%}{#sqrt{E}}"
if not calo_init.args.noLinearity:
    draw_text([formula], [0.55,0.8,0.95,0.95], colour, 0).SetTextSize(0.05)
else:
    draw_text([formula], [0.55,0.7,0.95,0.85], colour, 0).SetTextSize(0.05)
if calo_init.args.technical:
    constString = "const: "+str(round(fitResult.Get().Parameter(0),4))+" #pm "+str(round(fitResult.Get().Error(0),4))
    samplingString = "sampl: "+str(round(fitResult.Get().Parameter(1),4))+" #pm "+str(round(fitResult.Get().Error(1),4))
    draw_text([constString, samplingString], [0.55,0.68,0.88,0.76], colour+1, 0).SetTextSize(0.05)
draw_text(["energy resolution"], [0.2,0.88, 0.45,0.98], 1, 0).SetTextSize(0.05)
if calo_init.args.noLinearity and calo_init.args.specialLabel:
    draw_text([calo_init.args.specialLabel], [0.67,0.88, 0.95,0.98], kGray+3, 0).SetTextSize(0.05)
cRes.Update()
if not calo_init.args.noLinearity:
    padLin.cd()
    gLin.Draw("ape")
    draw_text(["linearity"], [0.2,0.78, 0.4,0.88], 1, 0).SetTextSize(0.05*factor)
    if calo_init.args.specialLabel:
        draw_text([calo_init.args.specialLabel], [0.67,0.78, 0.95,0.88], kGray+3, 0).SetTextSize(0.05*factor)

# Save canvas and root file with graph, const term and sampling term
if calo_init.output(0):
    cRes.SaveAs(calo_init.output(0)+".png")
    plots = TFile(calo_init.output(0)+".root","RECREATE")
else:
    cRes.SaveAs("energy_resolution_plots.gif")
    plots = TFile("energy_resolution_plots.root","RECREATE")
gRes.Write()
gLin.Write()
const = numpy.zeros(1, dtype=float)
sampl = numpy.zeros(1, dtype=float)
constErr = numpy.zeros(1, dtype=float)
samplErr = numpy.zeros(1, dtype=float)
t = TTree("params", "Fit parameters")
t.Branch("const", const, "const/D");
t.Branch("sampl", sampl, "sampl/D");
t.Branch("constErr", constErr, "constErr/D");
t.Branch("samplErr", samplErr, "samplErr/D");
const[0] = fitResult.Get().Parameter(0)
sampl[0] = fitResult.Get().Parameter(1)
constErr[0] = fitResult.Get().Error(0)
samplErr[0] = fitResult.Get().Error(1)
t.Fill()
plots.Write()
plots.Close()

raw_input("Press ENTER to exit")
