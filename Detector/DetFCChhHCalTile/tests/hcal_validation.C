
auto magic = fcc::MCParticleData();

auto _get_layerid = [](fcc::BareHit in) {
  int result;
  int systemId = in.cellId % 16;
  int layerId = (in.cellId >> 4) % 32;
  result = systemId * 100 + layerId;
  return result;


};

auto get_layerid = [](std::vector<fcc::PositionedCaloHitData> in) {

  std::vector<int> result;
  for (int i = 0; i < in.size(); ++i) {
  int layerId = (in[i].core.cellId >> (4+8+9)) % 32;
  result.push_back(layerId);
  }

return result;
};

auto get_systemid = [](std::vector<fcc::PositionedCaloHitData> in) {

  std::vector<int> result;
  for (int i = 0; i < in.size(); ++i) {

  int systemId = in[i].core.cellId % 16;
  result.push_back(systemId);
  }

return result;
};

auto get_z = [](std::vector<fcc::PositionedCaloHitData> in) {

  std::vector<float> result;
  for (int i = 0; i < in.size(); ++i) {
  result.push_back(in[i].position.z);
  }

return result;
};

auto get_r = [](std::vector<fcc::PositionedCaloHitData> in) {

  std::vector<float> result;
  for (int i = 0; i < in.size(); ++i) {
  result.push_back(std::sqrt(std::pow(in[i].position.y,2) + std::pow(in[i].position.x,2)));
  }

return result;
};

int hcal_validation(std::string filename) {
  gROOT->SetBatch();
  ROOT::RDataFrame df("events", filename);
  auto dff = df
               .Define("z", get_z, {"HCalBarrelPositionedHits"})
               .Define("r", get_r, {"HCalBarrelPositionedHits"})
               .Define("systemid", get_systemid, {"HCalBarrelPositionedHits"})
               .Define("layerid", get_layerid, {"HCalBarrelPositionedHits"})
               ;

  std::string basefilename(filename);
  basefilename.replace(basefilename.end() - 5, basefilename.end(), "");

  std::string snapshotname(filename);
  snapshotname.replace(snapshotname.end() - 5, snapshotname.end(), "_snapshot.root");

  /*
  TGraph *g;
  auto radiiPtr = dff.Take<float>({"r"});
  auto zPtr = dff.Take<float>({"z"});
  auto radii = *radiiPtr;
  auto zs = *zPtr;
    g = new TGraph(radii.size(), &zs[0], &radii[0]);
    g->Draw("AP");
    g->SetMarkerStyle(4);
    g->SetTitle(";z [mm]; r [mm]");
    g->SetName("rz_hit_graph");


   // This tutorial is ran with multithreading enabled. The order in which points are inserted is not known, so to have a meaningful representation points are sorted.
   TCanvas* c = new TCanvas();
    gPad->SetTicks();
   g->DrawClone();
   c->Print(graphfilename.c_str());

  */

  std::cout << "Write snapshot to " <<  snapshotname << " ... " <<  std::endl;
  dff.Snapshot("events", snapshotname, 
    {
      "z",
      "r",
      "systemid",
      "layerid",
      });

  // create graph
  std::string graphfilename = basefilename + "_rzgraph.png";
  TFile  f(snapshotname.c_str());
  
  TCanvas* c = new TCanvas();
  TTree* events = (TTree*)f.Get("events");
  int n = events->Draw("z:r", "","goff");
  TGraph *g = new TGraph(n,events->GetV1(),events->GetV2());
  g->SetTitle((basefilename + ";z [mm]; r[mm]").c_str());
  g->GetXaxis()->SetRangeUser(-350,350);
  g->GetYaxis()->SetRangeUser(2800,4500);
  g->Draw("ap");
  c->Print(graphfilename.c_str());

  gApplication->Terminate();
  return 0;


}
