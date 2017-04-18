#include "MaterialScan.h"
#include "DetInterface/IGeoSvc.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DDRec/MaterialManager.h"

#include "TFile.h"
#include "TMath.h"
#include "TTree.h"
#include "TVector3.h"

MaterialScan::MaterialScan(const std::string& name, ISvcLocator* svcLoc) : Service(name, svcLoc) {
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

  SmartIF<IRndmGenSvc> randSvc;
  randSvc = service("RndmGenSvc");
  StatusCode sc = m_flatPhiDist.initialize(randSvc, Rndm::Flat(0., M_PI / 2.));
  if (sc == StatusCode::FAILURE) {
    error() << "Unable to initialize random number generator." << endmsg;
    return sc;
  }


  std::unique_ptr<TFile> rootFile(TFile::Open(m_filename.value().c_str(), "RECREATE"));
  // no smart pointers possible because TTree is owned by rootFile (root mem management FTW!)
  TTree* tree = new TTree("materials", "");
  double eta = 0;
  double phi = 0;
  unsigned nMaterials = 0;
  std::unique_ptr<std::vector<double>> nX0(new std::vector<double>);
  std::unique_ptr<std::vector<double>> nLambda(new std::vector<double>);
  std::unique_ptr<std::vector<double>> matDepth(new std::vector<double>);
  std::unique_ptr<std::vector<std::string>> material(new std::vector<std::string>);
  auto nX0Ptr = nX0.get();
  auto nLambdaPtr = nLambda.get();
  auto matDepthPtr = matDepth.get();
  auto materialPtr = material.get();

  tree->Branch("eta", &eta);
  tree->Branch("nMaterials", &nMaterials);
  tree->Branch("nX0", &nX0Ptr);
  tree->Branch("nLambda", &nLambdaPtr);
  tree->Branch("matDepth", &matDepthPtr);
  tree->Branch("material", &materialPtr);

  auto lcdd = m_geoSvc->lcdd();
  DD4hep::DDRec::MaterialManager matMgr;
  DDSurfaces::Vector3D beginning(0, 0, 0);
  auto worldVol = lcdd->worldVolume()->GetShape();
  std::array<Double_t, 3> pos = {0, 0, 0};
  std::array<Double_t, 3> dir = {0, 0, 0};
  TVector3 vec(0, 0, 0);
  for (eta = -m_etaMax; eta < m_etaMax; eta += m_etaBinning) {
      for (int i = 0; i < m_nPhiTrials; ++i) {
      phi = m_flatPhiDist();
      nX0->clear();
      nLambda->clear();
      matDepth->clear();
      material->clear();

      std::map<DD4hep::Geometry::Material, double> phiAveragedMaterialsBetween;
      for (int iPhi = 0; iPhi < m_nPhiTrials; ++iPhi) {
          phi = m_flatPhiDist();
          //phi = 0;
        vec.SetPtEtaPhi(1, eta, phi);
        auto n = vec.Unit();
        dir = {n.X(), n.Y(), n.Z()};
        double distance = worldVol->DistFromInside(pos.data(), dir.data());

        DDSurfaces::Vector3D end(dir[0] * distance, dir[1] * distance, dir[2] * distance);
        debug() << "Calculating material between 0 and (" << end.x() << ", " << end.y() << ", " << end.z()
                << ") <=> eta = " << eta << ", phi =  " << phi  <<  endmsg;
        const DD4hep::DDRec::MaterialVec& materials = matMgr.materialsBetween(beginning, end);
        for (unsigned i = 0, n = materials.size(); i < n; ++i) {
          phiAveragedMaterialsBetween[materials[i].first] += materials[i].second / static_cast<double>(m_nPhiTrials);
      }
      }
      nMaterials = phiAveragedMaterialsBetween.size();
      for (auto matpair: phiAveragedMaterialsBetween) {
        TGeoMaterial* mat = matpair.first->GetMaterial();
        material->push_back(mat->GetName());
        matDepth->push_back(matpair.second ); 
        nX0->push_back(matpair.second / mat->GetRadLen()); 
        nLambda->push_back(matpair.second / mat->GetIntLen()); 
      }
    }
    tree->Fill();

  }
  tree->Write();
  rootFile->Close();
  return StatusCode::SUCCESS;
}

StatusCode MaterialScan::finalize() { return StatusCode::SUCCESS; }

DECLARE_SERVICE_FACTORY(MaterialScan)
