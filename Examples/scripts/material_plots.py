from __future__ import print_function
import argparse

from plotstyle import FCCStyle

import ROOT

def main():
    parser = argparse.ArgumentParser(description='Material Plotter')
    parser.add_argument('--fname', "-f", dest='fname', type=str, help="name of file to read")
    parser.add_argument('--etaMax', "-m", dest='etaMax', default=6, type=float, help="maximum pseudorapidity")
    parser.add_argument('--etaBin', "-b", dest='etaBin', default=0.05, type=float, help="pseudorapidity bin width")
    args = parser.parse_args()

    f = ROOT.TFile.Open(args.fname, "read")
    tree = f.Get("materials")
    histDict = {}

    # go through the eta bins and fill the histograms in the histDict, skipping air
    # keys in the histDict are the material names
    for etaBin, entry in enumerate(tree):
        nMat = entry.nMaterials
        for i in range(nMat):
            if entry.material.at(i) == "Air": continue
            if entry.material.at(i) not in histDict.keys():
                histDict[entry.material.at(i)] = {
                    "x0": ROOT.TH1F("", "", (int)(2 * args.etaMax / args.etaBin), -args.etaMax, args.etaMax),
                    "lambda": ROOT.TH1F("", "", (int)(2 * args.etaMax / args.etaBin), -args.etaMax, args.etaMax),
                    "depth": ROOT.TH1F("", "", (int)(2 * args.etaMax / args.etaBin), -args.etaMax, args.etaMax)
                }
            hs = histDict[entry.material.at(i)]
            hs["x0"].SetBinContent(etaBin+1, hs["x0"].GetBinContent(etaBin+1) + entry.nX0.at(i))
            hs["lambda"].SetBinContent(etaBin+1, hs["lambda"].GetBinContent(etaBin+1) + entry.nLambda.at(i))
            hs["depth"].SetBinContent(etaBin+1, hs["depth"].GetBinContent(etaBin+1) + entry.matDepth.at(i))

    axis_titles = ["Number of X_{0}", "Number of #lambda", "Material depth [cm]"]

    # This loop does the drawing, sets the style and saves the pdf files
    for plot, title in zip(["x0", "lambda", "depth"], axis_titles):
        legend = ROOT.TLegend(.75, .75, .94, .94)
        legend.SetLineColor(0)
        ths = ROOT.THStack()
        for i, material in enumerate(histDict.keys()):
            linecolor = 1
            fillcolor = FCCStyle.fillcolors[i]
            histDict[material][plot].SetLineColor(linecolor)
            histDict[material][plot].SetFillColor(fillcolor)
            histDict[material][plot].SetLineWidth(1)
            histDict[material][plot].SetFillStyle(1001)

            ths.Add(histDict[material][plot])
            legend.AddEntry(histDict[material][plot], material, "f")

        cv = ROOT.TCanvas()
        ths.Draw()
        ths.GetXaxis().SetTitle("#eta")
        ths.GetYaxis().SetTitle(title)

        legend.Draw()
        cv.Print(plot + ".pdf")

        ths.GetXaxis().SetRangeUser(0, args.etaMax)
        cv.Print(plot + "pos.pdf")

if __name__ == "__main__":
    FCCStyle.initialize()
    main()
