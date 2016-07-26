#include "SimG4ParticleSmearRootFile.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmGen.h"

//ROOT
#include "TFile.h"
#include "TTree.h"
#include "TArray.h"

//CLHEP
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

DECLARE_TOOL_FACTORY(SimG4ParticleSmearRootFile)

SimG4ParticleSmearRootFile::SimG4ParticleSmearRootFile(const std::string& type, const std::string& name, const IInterface* parent):
    GaudiTool(type, name, parent),
    m_maxEta(0) {
  declareInterface<ISimG4ParticleSmearTool>(this);
  declareProperty("filename", m_resolutionFileName);
}

SimG4ParticleSmearRootFile::~SimG4ParticleSmearRootFile() {}

StatusCode SimG4ParticleSmearRootFile::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_randSvc = service("RndmGenSvc");
  if(!m_randSvc) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  if(readResolutions().isFailure()) {
    error() << "Couldn't read the input resolution file from tkLayout" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4ParticleSmearRootFile::finalize() {
  return GaudiTool::finalize();
}

StatusCode SimG4ParticleSmearRootFile::smearMomentum( CLHEP::Hep3Vector& aMom, int /*aPdg*/) {
  double res = resolution(aMom.pseudoRapidity(), aMom.mag()/CLHEP::GeV);
  if(res>0) {
    m_randSvc->generator(Rndm::Gauss(1,res), m_gauss);
    double tmp = m_gauss->shoot();
    aMom *= tmp;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4ParticleSmearRootFile::smearEnergy( double& /*aEn*/, int /*aPdg*/) {
  warning()<<"Root file smearing is meant to be used with tracker only,"<<endmsg;
  warning()<<"hence smearing can be performed for the momentum only!"<<endmsg;
  return StatusCode::FAILURE;
}

StatusCode SimG4ParticleSmearRootFile::readResolutions() {
  // check if file exists
  if (m_resolutionFileName.empty()) {
    error() << "Name of the resolution file not set" << endmsg;
    return StatusCode::FAILURE;
  }
  TFile f(m_resolutionFileName.c_str(),"READ");
  if (f.IsZombie()) {
    error() << "Couldn't open the resolution file" << endmsg;
    return StatusCode::FAILURE;
  }
  // check the proper file structure
  if(! (f.GetListOfKeys()->Contains("info") && f.GetListOfKeys()->Contains("resolutions") )) {
    error()<<"Resolution file "<<m_resolutionFileName<<" does not contain trees <<info>> and <<resolutions>>"<<endmsg;
    return StatusCode::FAILURE;
  }
  // retrieve the pseudorapidity and momentum values for which the resolutions are defined
  TTree* infoTree = dynamic_cast<TTree*>(f.Get("info"));
  // check the proper tree structure
  if(! (infoTree->GetListOfBranches()->Contains("eta") && infoTree->GetListOfBranches()->Contains("p") )) {
    error()<<"Resolution file "<<m_resolutionFileName<<" does not contain tree <<info>>"
           <<" with branches <<eta>> and <<p>>"<<endmsg;
    return StatusCode::FAILURE;
  }
  TArrayD* readEta = nullptr;
  TArrayD* readP = nullptr;
  infoTree->SetBranchAddress("eta",&readEta);
  infoTree->SetBranchAddress("p",&readP);
  infoTree->GetEntry(0);
  int binsEta = readEta->GetSize();
  int binsP = readP->GetSize();
  double minP = readP->At(0);
  double maxP = readP->At(binsP-1);
  double maxEta = readEta->At(binsEta-1);
  info()<<"Reading from resolutions file: "<<f.GetName()<<endmsg;
  info()<<"\tMaximum pseudorapidity: "<<maxEta<<endmsg;
  info()<<"\tMinimum momentum: "<<minP<<endmsg;
  info()<<"\tMaximum momentum: "<<maxP<<endmsg;

  TTree* resolutionTree = dynamic_cast<TTree*>(f.Get("resolutions"));
  // check the proper tree structure
  if(! (resolutionTree->GetListOfBranches()->Contains("resolution"))) {
    error()<<"Resolution file "<<m_resolutionFileName<<" does not contain tree <<resolutions>>"
           <<" with branch <<resolution>>"<<endmsg;
    return StatusCode::FAILURE;
  }
  TArrayD* readRes = nullptr;
  resolutionTree->SetBranchAddress("resolution",&readRes);
  for(int itEta=0; itEta<binsEta; itEta++) {
    resolutionTree->GetEntry(itEta);
    m_momentumResolutions[readEta->At(itEta)] = TGraph(binsP, readP->GetArray(), readRes->GetArray());
    if(msgLevel(MSG::DEBUG)) {
      debug()<<"resolutions for eta ("<<(itEta==0?0:readEta->At(itEta-1))<<", "<<readEta->At(itEta)<<"): \n";
      for (int iP=0; iP<binsP; ++iP) {
        debug()<<"(p = "<<readP->At(iP)<<" GeV) -> "<<readRes->At(iP)<<" %\t";
      }
      debug()<<endmsg;
    }
  }
  f.Close();
  return StatusCode::SUCCESS;
}

double SimG4ParticleSmearRootFile::resolution(double aEta, double aMom) {
  // smear particles only in the pseudorapidity region where resolutions are defined
  if(fabs(aEta)>m_maxEta)
    return 0;

  for(auto& m: m_momentumResolutions) {
    if(fabs(aEta)<m.first) {
      return m.second.Eval(aMom);
    }
  }
  return 0;
}
