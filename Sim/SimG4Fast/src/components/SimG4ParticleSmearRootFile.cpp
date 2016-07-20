#include "SimG4ParticleSmearRootFile.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmGen.h"

//ROOT
#include "TFile.h"
#include "TList.h"
#include "TGraph.h"

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
  if (m_resolutionFileName.empty()) {
    error() << "Name of the resolution file not set" << endmsg;
    return StatusCode::FAILURE;
  }
  TFile f(m_resolutionFileName.c_str(),"READ");
  if (f.IsZombie()) {
    error() << "Couldn't open the resolution file" << endmsg;
    return StatusCode::FAILURE;
  }
  TObjLink *lnk = f.GetListOfKeys()->FirstLink();
  double lowEta=0, highEta=0;
  debug()<<"Reading from resolutions file: "<<f.GetName()<<endmsg;
  while(lnk) {
    if(sscanf(lnk->GetObject()->GetName(), "etafrom%lf_etato%lf", &lowEta, &highEta)==2) {
      m_momentumResolutions[highEta] = std::unique_ptr<TGraph>((TGraph*)f.Get(lnk->GetObject()->GetName())->Clone());
    }
    lnk=lnk->Next();
  }
  m_maxEta = (--m_momentumResolutions.end())->first;
  debug()<<"maximum pseudorapidity value of the read resolutions: "<<m_maxEta<<endmsg;
  f.Close();
  // TODO here sort map in eta to make sure it is increasing
  // (it is now because of the way it is produced, but one never knows...)
  return StatusCode::SUCCESS;
}

double SimG4ParticleSmearRootFile::resolution(double aEta, double aMom) {
  // smear particles only in the pseudorapidity region where resolutions are defined
  if(fabs(aEta)>m_maxEta)
    return 0;

  for(auto& m: m_momentumResolutions) {
    if(fabs(aEta)<m.first) {
      return m.second.get()->Eval(aMom);
    }
  }
  return 0;
}
