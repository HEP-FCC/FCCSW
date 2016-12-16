#include "MaterialScan.h"
#include "DetInterface/IGeoSvc.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/ITHistSvc.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DDRec/MaterialManager.h"

#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"


MaterialScan::MaterialScan(const std::string& name, ISvcLocator* svcLoc): Service(name, svcLoc) {
    declareProperty("filename", m_filename, "file name to save the tree to");
    declareProperty("etaBinning", m_etaBinning, "eta bin size");
    declareProperty("etaMax", m_etaMax, "maximum eta value");
}

StatusCode MaterialScan::initialize() {
  if (Service::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (nullptr == m_geoSvc) {
    error() << "Unable to find Geometry Service." << endmsg;
    return StatusCode::FAILURE;
  }

  std::unique_ptr<TFile> rootFile(TFile::Open(m_filename.c_str(), "RECREATE"));
  // no smart pointers possible because TTree is owned by rootFile (root mem management FTW!)
  TTree* tree = new TTree("materials", "");
  double eta = 0;
  unsigned nMaterials = 0;
  std::unique_ptr<std::vector<double>> nX0(new std::vector<double>);
  std::unique_ptr<std::vector<double>> nLambda(new std::vector<double>);
  std::unique_ptr<std::vector<double>> matDepth(new std::vector<double>);
  std::unique_ptr<std::vector<std::string>> material(new std::vector<std::string>);
  tree->Branch("eta", &eta);
  tree->Branch("nMaterials", &nMaterials);
  tree->Branch("nX0", &nX0);
  tree->Branch("nLambda", &nLambda);
  tree->Branch("matDepth", &matDepth);
  tree->Branch("material", &material);

  auto lcdd = m_geoSvc->lcdd();
  DD4hep::DDRec::MaterialManager matMgr;
  DDSurfaces::Vector3D beginning(0, 0, 0);
  auto worldVol = lcdd->worldVolume()->GetShape();
  std::array<Double_t, 3> pos = {0, 0, 0};
  std::array<Double_t, 3> dir = {0, 0, 0};
  TVector3 vec(0, 0, 0);
  for (eta = -m_etaMax; eta < m_etaMax; eta += m_etaBinning) {
    nX0->clear();
    nLambda->clear();
    matDepth->clear();
    material->clear();

    vec.SetPtEtaPhi(1, eta, 0);
    auto n = vec.Unit();
    dir = {n.X(), n.Y(), n.Z()};
    double distance = worldVol->DistFromInside(pos.data(), dir.data());

    DDSurfaces::Vector3D end(dir[0]*distance, dir[1]*distance, dir[2]*distance);
    debug() << "Calculating material between 0 and (" <<end.x()  << ", " << end.y() << ", " << end.z() << ") <=> eta = " << eta << endmsg;
    const DD4hep::DDRec::MaterialVec& materials = matMgr.materialsBetween(beginning, end);
    nMaterials = materials.size();

    for (unsigned i=0, n = materials.size(); i < n; ++i){
      TGeoMaterial* mat =  materials[i].first->GetMaterial();
      material->push_back(mat->GetName());
      matDepth->push_back(materials[i].second);
      nX0->push_back(materials[i].second / mat->GetRadLen());
      nLambda->push_back(materials[i].second / mat->GetIntLen());
    }
    tree->Fill();
  }
  tree->Write();
  rootFile->Close();
  return StatusCode::SUCCESS;
}

StatusCode MaterialScan::finalize() {return StatusCode::SUCCESS;}

DECLARE_SERVICE_FACTORY(MaterialScan)
