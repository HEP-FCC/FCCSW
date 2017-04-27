#include "MomentumRangeParticleGun.h"

#include <cmath>

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "HepMC/GenEvent.h"

DECLARE_TOOL_FACTORY(MomentumRangeParticleGun)

/// Constructor
MomentumRangeParticleGun::MomentumRangeParticleGun(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IParticleGunTool>(this);
}

/// Destructor
MomentumRangeParticleGun::~MomentumRangeParticleGun() {}

/// Initialize Particle Gun parameters
StatusCode MomentumRangeParticleGun::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (!sc.isSuccess()) return sc;

  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = m_flatGenerator.initialize(randSvc, Rndm::Flat(0., 1.));
  if (!sc.isSuccess()) return Error("Cannot initialize flat generator");

  // Get the mass of the particle to be generated
  //
  IParticlePropertySvc* ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc", true);

  // check momentum and angles
  if ((m_minMom > m_maxMom) || (m_minTheta > m_maxTheta) || (m_minPhi > m_maxPhi))
    return Error("Incorrect values for momentum, theta or phi!");

  m_deltaMom = m_maxMom - m_minMom;
  m_deltaPhi = m_maxPhi - m_minPhi;
  m_deltaTheta = m_maxTheta - m_minTheta;

  // setup particle information
  m_masses.clear();

  info() << "Particle type chosen randomly from :";
  PIDs::iterator icode;
  for (icode = m_pdgCodes.begin(); icode != m_pdgCodes.end(); ++icode) {
    const ParticleProperty* particle = ppSvc->findByPythiaID(*icode);
    m_masses.push_back((particle->mass()));
    m_names.push_back(particle->particle());
    info() << " " << particle->particle();
  }

  info() << endmsg;

  info() << "Momentum range: " << m_minMom / Gaudi::Units::GeV << " GeV <-> " << m_maxMom / Gaudi::Units::GeV << " GeV"
         << endmsg;
  info() << "Theta range: " << m_minTheta / Gaudi::Units::rad << " rad <-> " << m_maxTheta / Gaudi::Units::rad << " rad"
         << endmsg;
  info() << "Phi range: " << m_minPhi / Gaudi::Units::rad << " rad <-> " << m_maxPhi / Gaudi::Units::rad << " rad"
         << endmsg;

  release(ppSvc);

  return sc;
}

/// Generate the particles
void MomentumRangeParticleGun::generateParticle(Gaudi::LorentzVector& momentum,
                                                Gaudi::LorentzVector& origin,
                                                int& pdgId) {

  origin.SetCoordinates(0., 0., 0., 0.);
  double px(0.), py(0.), pz(0.);

  // Generate values for energy, theta and phi
  double p = m_minMom + m_flatGenerator() * (m_deltaMom);
  double theta = m_minTheta + m_flatGenerator() * (m_deltaTheta);
  double phi = m_minPhi + m_flatGenerator() * (m_deltaPhi);

  // Transform to x,y,z coordinates
  double pt = p * sin(theta);
  px = pt * cos(phi);
  py = pt * sin(phi);
  pz = p * cos(theta);

  // randomly choose a particle type
  unsigned int currentType = (unsigned int)(m_pdgCodes.size() * m_flatGenerator());
  // protect against funnies
  if (currentType >= m_pdgCodes.size()) currentType = 0;

  momentum.SetPx(px);
  momentum.SetPy(py);
  momentum.SetPz(pz);
  momentum.SetE(std::sqrt(m_masses[currentType] * m_masses[currentType] + momentum.P2()));

  pdgId = m_pdgCodes[currentType];

  debug() << " -> " << m_names[currentType] << endmsg << "   P   = " << momentum << endmsg;
}

StatusCode MomentumRangeParticleGun::getNextEvent(HepMC::GenEvent& theEvent) {
  Gaudi::LorentzVector theFourMomentum;
  Gaudi::LorentzVector origin;
  // note: pgdid is set in function generateParticle
  int thePdgId;
  generateParticle(theFourMomentum, origin, thePdgId);

  // create HepMC Vertex --
  // by calling add_vertex(), the hepmc event is given ownership
  //  of the vertex
  HepMC::GenVertex* v = new HepMC::GenVertex(HepMC::FourVector(origin.X(), origin.Y(), origin.Z(), origin.T()));
  // create HepMC particle --
  // by calling add_particle_out(), the hepmc vertex is given ownership
  // of the particle
  HepMC::GenParticle* p = new HepMC::GenParticle(
      HepMC::FourVector(theFourMomentum.Px(), theFourMomentum.Py(), theFourMomentum.Pz(), theFourMomentum.E()),
      thePdgId,
      1);  // hepmc status code for final state particle

  v->add_particle_out(p);

  theEvent.add_vertex(v);
  theEvent.set_signal_process_vertex(v);

  return StatusCode::SUCCESS;
}
