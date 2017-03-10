#include "TSystem.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TKey.h"
#include "TCanvas.h"
#include "TMultiGraph.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TMath.h"
#include "TEventList.h"
#include "TStyle.h"
#include "THStack.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TView.h"
#include "TGeoManager.h"

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


using namespace std;

void testTrapezoid()
{
  gSystem->Load("libGeo");
  TCanvas *c = new TCanvas("c", "c",0,0,600,600);
  TGeoManager *geom =  new TGeoManager("trd2", "poza9");
  TGeoMaterial *mat = new TGeoMaterial("Al", 26.98,13,2.7);
  TGeoMedium *med = new TGeoMedium("MED",1,mat);
   TGeoVolume *top = gGeoManager->MakeBox("TOP",med,100,100,100);
   gGeoManager->SetTopVolume(top);
  TGeoVolume *vol = geom->MakeTrd2("Trd2",med, 1,2,30,10,40);
  vol->SetLineWidth(2);
  gGeoManager->CloseGeometry();
  gGeoManager->SetNsegments(80);
  vol->Draw();
  TView *view = gPad->GetView();
  view->ShowAxis();
}
