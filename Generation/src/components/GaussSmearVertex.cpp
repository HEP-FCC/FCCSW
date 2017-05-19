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

  declareProperty("xVertexSigma", m_xsig = 0.0 * Gaudi::Units::mm);
  declareProperty("yVertexSigma", m_ysig = 0.0 * Gaudi::Units::mm);
  declareProperty("zVertexSigma", m_zsig = 0.0 * Gaudi::Units::mm);
  declareProperty("BeamDirection", m_zDir = 0);
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


  m_xsig = m_xmax - m_xmin;
  m_xmean = m_xmin + m_xsig * 0.5;

  m_ysig = m_ymax - m_ymin;
  m_ymean = m_ymin + m_ysig * 0.5;

  m_zsig = m_zmax - m_zmin;
  m_zmean = m_zmin + m_zsig * 0.5;

  m_tsig = m_tmax - m_tmin;
  m_tmean = m_tmin + m_tsig * 0.5;

  sc = m_gaussDist.initialize(randSvc, Rndm::Gauss(0., 1));
  if (sc.isFailure()) return sc;

  std::string infoMsg = " applying TOF of interaction with ";
  if (m_zDir == -1) {
    infoMsg = infoMsg + "negative beam direction";
  } else if (m_zDir == 1) {
    infoMsg = infoMsg + "positive beam direction";
  } else if (m_zDir == 0) {
    infoMsg = " with TOF of interaction equal to zero ";
  } else {
    return Error("BeamDirection can only be set to -1 or 1, or 0 to switch off TOF");
  }

  info() << "Smearing of interaction point with normal distribution "
         << " in x, y and z " << endmsg;
  info() << infoMsg << endmsg;
  info() << " with " << m_xsig / Gaudi::Units::mm << " mm  standard deviation in x " << m_ysig / Gaudi::Units::mm
         << " mm in y and " << m_zsig / Gaudi::Units::mm << " mm in z." << endmsg;

  if (!sc.isSuccess()) return Error("Could not initialize normal random number generator");

  release(randSvc);
  return sc;
}

/// Smearing function
StatusCode GaussSmearVertex::smearVertex(HepMC::GenEvent& theEvent) {
  double dx, dy, dz, dt;

  dx = m_gaussDist() * sqrt(m_xsig) + m_xmean;
  dy = m_gaussDist() * sqrt(m_ysig) + m_ymean;
  dz = m_gaussDist() * sqrt(m_zsig) + m_zmean;
  dt = m_gaussDist() * sqrt(m_tsig)  + m_tmean;

  Gaudi::LorentzVector dpos(dx, dy, dz, dt);

  debug() << "Smearing vertices by " << dpos << endmsg;

  for (auto vit = theEvent.vertices_begin(); vit != theEvent.vertices_end(); ++vit) {
    Gaudi::LorentzVector pos((*vit)->position());
    pos += dpos;
    (*vit)->set_position(HepMC::FourVector(pos.x(), pos.y(), pos.z(), pos.t()));
  }

  return StatusCode::SUCCESS;
}
