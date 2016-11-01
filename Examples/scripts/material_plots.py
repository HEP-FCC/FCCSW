from __future__ import print_function
import argparse

from plotstyle import FCCStyle

import ROOT

def main():
    parser = argparse.ArgumentParser(description='Material Plotter')
    parser.add_argument('--fname', "-f", dest='fname', type=str, help="file name to read")
    args = parser.parse_args()

    f = ROOT.TFile.Open(args.fname, "read")
    tree = f.Get("materials")
    histDict = {}
    for etaBin, entry in enumerate(tree):
        nMat = entry.nMaterials
        for i in range(nMat):
            if entry.material.at(i) == "Air": continue
            if entry.material.at(i) not in histDict.keys():
                histDict[entry.material.at(i)] = {
                    "x0": ROOT.TH1F("", "", 240, -6, 6),
                    "lambda": ROOT.TH1F("", "", 240, -6, 6),
                    "depth": ROOT.TH1F("", "", 240, -6, 6)
                }
            hs = histDict[entry.material.at(i)]
            hs["x0"].SetBinContent(etaBin+1, hs["x0"].GetBinContent(etaBin+1) + entry.nX0.at(i))
            hs["lambda"].SetBinContent(etaBin+1, hs["lambda"].GetBinContent(etaBin+1) + entry.nLambda.at(i))
            hs["depth"].SetBinContent(etaBin+1, hs["depth"].GetBinContent(etaBin+1) + entry.matDepth.at(i))

    axis_titles = ["Number of X_{0}", "Number of #lambda", "Material depth [cm]"]

    for plot, title in zip(["x0", "lambda", "depth"], axis_titles):
        legend = ROOT.TLegend(.75, .75, .94, .94)
        legend.SetLineColor(0)
        ths = ROOT.THStack()
        for i, material in enumerate(histDict.keys()):
            linecolor = 1
            fillcolor = FCCSyle.fillcolors[i]
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

        ths.GetXaxis().SetRangeUser(0, 6)
        cv.Print(plot + "pos.pdf")

if __name__ == "__main__":
    FCCStyle.initialize()
    main()
