#include "G4ParticleSmearTklayout.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

//ROOT
#include "TFile.h"
#include "TList.h"
#include "TGraph.h"

//CLHEP
#include "CLHEP/Vector/ThreeVector.h"

DECLARE_TOOL_FACTORY(G4ParticleSmearTklayout)

G4ParticleSmearTklayout::G4ParticleSmearTklayout(const std::string& type, const std::string& name, const IInterface* parent):
    GaudiTool(type, name, parent),
    m_maxEta(0) {
  declareInterface<IG4ParticleSmearTool>(this);
  declareProperty("filename", m_resolutionFileName = "");
}

G4ParticleSmearTklayout::~G4ParticleSmearTklayout() {}

StatusCode G4ParticleSmearTklayout::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if(service( "RndmGenSvc" , m_randSvc ).isFailure()) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  if(readResolutions().isFailure()) {
    error() << "Couldn't read the input resolution file from tkLayout" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode G4ParticleSmearTklayout::finalize() {
  return GaudiTool::finalize();
}

StatusCode G4ParticleSmearTklayout::smearMomentum( CLHEP::Hep3Vector& aMom, int /*aPdg*/) {
  double res = resolution(aMom.pseudoRapidity(), aMom.perp());
  if(res>0) {
    m_gauss.initialize(m_randSvc, Rndm::Gauss(1,res));
    double tmp = m_gauss.shoot();
    aMom *= tmp;
  }
  return StatusCode::SUCCESS;
}

StatusCode G4ParticleSmearTklayout::smearEnergy( double& aEn, int /*aPdg*/) {
  warning()<<"TkLayout smearing is meant to be used with tracker only,"<<endmsg;
  warning()<<"hence smearing can be performed for the momentum only!"<<endmsg;
  return StatusCode::FAILURE;
}

StatusCode G4ParticleSmearTklayout::readResolutions() {
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
  // TODO here sort map in eta to make sure it is increasing
  // (it is now because of the way it is produced, but one never knows...)
  return StatusCode::SUCCESS;
}

double G4ParticleSmearTklayout::resolution(double aEta, double aMom) {
  // smear particles only in the pseudorapidity region where resolutions are defined
  if(fabs(aEta)>m_maxEta)
    return 0;

  for(auto& m: m_momentumResolutions) {
    if(fabs(aEta)<m.first) {
      return m.second.get()->Eval(aPt);
    }
  }
  return 0;
}
