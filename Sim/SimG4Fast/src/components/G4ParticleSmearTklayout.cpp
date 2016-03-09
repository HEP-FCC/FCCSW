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
  GaudiTool(type, name, parent) {
  declareInterface<IG4ParticleSmearTool>(this);
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
  return StatusCode::SUCCESS;
}

StatusCode G4ParticleSmearTklayout::finalize() {
  readResolutions();
  return GaudiTool::finalize();
}

StatusCode G4ParticleSmearTklayout::smearMomentum( CLHEP::Hep3Vector& aMom, int /*aPdg*/) {
  // double tmp = m_gauss.shoot();
  // aMom *= tmp;
  return StatusCode::SUCCESS;
}

StatusCode G4ParticleSmearTklayout::smearEnergy( double& aEn, int /*aPdg*/) {
  // double tmp;
  // do {
  //   tmp = m_gauss.shoot();
  //   aEn *= tmp;
  // }
  // while(aEn<0);
  return StatusCode::SUCCESS;
}
// double G4ParticleSmearTklayout::resolution(double aEta, double aPt) {

//   for map of eta
//             if(aEta>=itEta->fi)
//               map.second->Eval
//                 }

StatusCode G4ParticleSmearTklayout::readResolutions() {
  TFile f("res.root","READ");
  TList* l = f.GetListOfKeys();
  TObjLink *lnk = f.GetListOfKeys()->FirstLink();
  TIter iter(l);
  while(lnk) {
    debug()<<f.Get(lnk->GetObject()->GetName())->ClassName()<<endmsg;
    if(!std::strcmp(f.Get(lnk->GetObject()->GetName())->ClassName(), "TGraph"))
      debug()<<"equal!"<<endmsg;
    lnk=lnk->Next();
    }
  return StatusCode::SUCCESS;

}
