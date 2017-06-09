#include "GaussSmearVertex.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Vector4DTypes.h"

/// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY(GaussSmearVertex)

/// Standard constructor, initializes variables
GaussSmearVertex::GaussSmearVertex(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IVertexSmearingTool>(this);

}

/// Destructor
GaussSmearVertex::~GaussSmearVertex() { ; }

//=============================================================================
// Initialize
//=============================================================================
StatusCode GaussSmearVertex::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);



  sc = m_gaussDist.initialize(randSvc, Rndm::Gauss(0., 1));
  if (sc.isFailure()) return sc;


  info() << "Smearing of interaction point with normal distribution "
         << " in x, y and z " << endmsg;
  info() << " with " << m_xsig / Gaudi::Units::mm << " mm  standard deviation in x " << m_ysig / Gaudi::Units::mm
         << " mm in y and " << m_zsig / Gaudi::Units::mm << " mm in z." << endmsg;

  if (!sc.isSuccess()) return Error("Could not initialize normal random number generator");

  release(randSvc);
  return sc;
}

/// Smearing function
StatusCode GaussSmearVertex::smearVertex(HepMC::GenEvent& theEvent) {

  double dx = m_gaussDist() * sqrt(m_xsig) + m_xmean;
  double dy = m_gaussDist() * sqrt(m_ysig) + m_ymean;
  double dz = m_gaussDist() * sqrt(m_zsig) + m_zmean;
  double dt = m_gaussDist() * sqrt(m_tsig) + m_tmean;

  Gaudi::LorentzVector dpos(dx, dy, dz, dt);

  debug() << "Smearing vertices by " << dpos << endmsg;

  for (auto vit = theEvent.vertices_begin(); vit != theEvent.vertices_end(); ++vit) {
    Gaudi::LorentzVector pos((*vit)->position());
    pos += dpos;
    (*vit)->set_position(HepMC::FourVector(pos.x(), pos.y(), pos.z(), pos.t()));
  }

  return StatusCode::SUCCESS;
}
