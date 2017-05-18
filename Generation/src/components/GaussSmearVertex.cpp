#include "GaussSmearVertex.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
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

  sc = m_gaussDistX.initialize(randSvc, Rndm::Gauss(0., m_xsig));
  if (sc.isFailure()) return sc;
  sc = m_gaussDistY.initialize(randSvc, Rndm::Gauss(0., m_ysig));
  if (sc.isFailure()) return sc;
  sc = m_gaussDistZ.initialize(randSvc, Rndm::Gauss(0., m_zsig));

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

  dx = m_gaussDistX();
  dy = m_gaussDistY();
  dz = m_gaussDistZ();
  dt = m_zDir * dz / Gaudi::Units::c_light;

  Gaudi::LorentzVector dpos(dx, dy, dz, dt);

  debug() << "Smearing vertices by " << dpos << endmsg;

  for (auto vit = theEvent.vertices_begin(); vit != theEvent.vertices_end(); ++vit) {
    Gaudi::LorentzVector pos((*vit)->position());
    pos += dpos;
    (*vit)->set_position(HepMC::FourVector(pos.x(), pos.y(), pos.z(), pos.t()));
  }

  return StatusCode::SUCCESS;
}
