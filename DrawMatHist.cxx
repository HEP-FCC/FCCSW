//
//  DrawMatHist.cxx
//  
//
//  Created by Julia Hrdinka on 28/05/15.
//
//

void DrawMatHist ()
{
    TCanvas* c1 = new TCanvas("c1","example",600,700);
     gStyle->SetOptStat(0);
    TFile G4f("geant4detector.root");
    TProfile* p2 = (TProfile*)G4f.Get("tInX01");
    
    TProfile* p1 = (TProfile*)G4f.Get("G4::tInX0");
 //   TProfile* p2 = (TProfile*)f.Get("tInX0");
   
    p2->SetFillColor(8);
    p2->SetLineColor(8);
    p2->SetMarkerColor(8);
    p2->SetMarkerStyle(8);
//    p2->Rebin(10);
    p2->Draw();
    p2->GetXaxis()->SetTitle("#eta");
    p2->GetYaxis()->SetTitle("t/x0");

    p1->SetFillColor(2);
    p1->SetMarkerColor(2);
    p1->SetLineColor(2);
//    p1->Rebin(10);
//    p1->SetLineStyle(2);
    p1->SetMarkerStyle(24)
//p1->SetFillStyle(3001);
    p1->Draw("same");
    TLegend *leg = new TLegend(0.4,0.6,0.89,0.89);
    leg->AddEntry(p1,"full geometry","f");
    leg->AddEntry(p2,"reconstruction geometry","f");
    leg->Draw();
    
//    TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.3);
//    TH1F* h2 = (TH1F*)G4f.Get("tInX01")
//    TH1F* h1 = (TH1F*)G4f.Get("G4::tInX0")
    //h1->Rebin(20)
    //h2->Rebin(20)
//    h1->Divide(h2)
//    h1->GetXaxis()->SetTitle("Eta")
//    h1->SetTitle("Fraction Plot")
//    h1->Draw()


 //    TFile f("hitproperies.root");;

TCanvas* c2 = new TCanvas();
TProfile* p2 = (TProfile*)G4f.Get("Pathlength1");

TProfile* p1 = (TProfile*)G4f.Get("G4::Pathlength");
//   TProfile* p2 = (TProfile*)f.Get("tInX0");

p2->SetFillColor(8);
p2->SetLineColor(8);
p2->SetMarkerColor(8);
p2->SetMarkerStyle(8);
p2->Draw();

p1->SetFillColor(2);
p1->SetMarkerColor(2);
p1->SetLineColor(2);
p1->SetMarkerStyle(24)
p1->Draw("same");
    TLegend *leg = new TLegend(0.4,0.6,0.89,0.89);
    leg->AddEntry(p1,"full geometry","f");
    leg->AddEntry(p2,"reconstruction geometry","f");
    leg->Draw();

TCanvas* c3 = new TCanvas();
TProfile* p2 = (TProfile*)G4f.Get("Hits");

TProfile* p1 = (TProfile*)G4f.Get("G4::Hits");
//   TProfile* p2 = (TProfile*)f.Get("tInX0");

p2->SetFillColor(49);
p2->SetLineColor(49);
p2->SetMarkerColor(49);
p2->Draw();

p1->SetFillColor(44);
p1->SetMarkerColor(44);
p1->SetLineColor(44);
p1->Draw("same");
///////
    
    //  h1->GetYaxis()->SetTickSize(0.25)
    //  h2->GetYaxis()->SetTickSize(0.25)
    //h1->Rebin(20)
    //h2->Rebin(20)
    

TFile G4f("geant4detector.root")
TH1F* h2 = (TH1F*)G4f.Get("tInX01")
TH1F* h1 = (TH1F*)G4f.Get("G4::tInX0")
h1->GetYaxis()->SetRangeUser(0.,4.)
h2->GetYaxis()->SetRangeUser(0.,4.)
h1->Sumw2()
h2->Sumw2()
h1->Divide(h2)
h1->GetXaxis()->SetTitle("Eta")
h1->GetYaxis()->SetTitle("ratio")
h1->GetYaxis()->SetNdivisions(4)
h1->GetYaxis()->SetLabelSize(0.065)
h1->GetXaxis()->SetLabelSize(0.055)
h1->GetXaxis()->SetTitleSize(0.070)
h1->GetYaxis()->SetTitleSize(0.080)
h1->GetYaxis()->SetTitleOffset(0.55)
h1->GetXaxis()->SetTitleOffset(0.6)
h1->SetTitle("")
h1->Draw()

TLine* line = new TLine(-4,1,4,1);
    line->SetLineColor(2);
line->SetLineWidth(2)
line->Draw()