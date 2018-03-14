#include "ConstPtParticleGun.h"

#include <cmath>

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "HepMC/GenEvent.h"
#include "HepPDT/ParticleID.hh"
#include "Pythia8/ParticleData.h"

DECLARE_TOOL_FACTORY(ConstPtParticleGun)

/// Constructor
ConstPtParticleGun::ConstPtParticleGun(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IParticleGunTool>(this);
}

/// Destructor
ConstPtParticleGun::~ConstPtParticleGun() {}

/// Initialize Particle Gun parameters
StatusCode ConstPtParticleGun::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (!sc.isSuccess()) return sc;

  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = m_flatGenerator.initialize(randSvc, Rndm::Flat(0., 1.));
  if (!sc.isSuccess()) return Error("Cannot initialize flat generator");

  // check momentum and angles
  if ((m_minEta > m_maxEta) || (m_minPhi > m_maxPhi)) return Error("Incorrect values for eta or phi!");

  m_deltaPhi = m_maxPhi - m_minPhi;
  m_deltaEta = m_maxEta - m_minEta;

  // setup particle information
  m_masses.clear();
  auto pd = Pythia8::ParticleData();

  info() << "Particle type chosen randomly from :";
  PIDs::iterator icode;
  for (icode = m_pdgCodes.begin(); icode != m_pdgCodes.end(); ++icode) {
    info() << " " << *icode;
    m_masses.push_back(pd.m0(*icode));
  }

  info() << endmsg;

  info() << "Eta range: " << m_minEta << "  <-> " << m_maxEta << endmsg;
  info() << "Phi range: " << m_minPhi / Gaudi::Units::rad << " rad <-> " << m_maxPhi / Gaudi::Units::rad << " rad"
         << endmsg;

  return sc;
}

/// Generate the particles
void ConstPtParticleGun::generateParticle(Gaudi::LorentzVector& momentum, Gaudi::LorentzVector& origin, int& pdgId) {

  origin.SetCoordinates(0., 0., 0., 0.);
  double px(0.), py(0.), pz(0.);

  // Generate values for eta  and phi
  double phi = m_minPhi + m_flatGenerator() * (m_deltaPhi);
  double eta = m_minEta + m_flatGenerator() * (m_deltaEta);

  // Transform to x,y,z coordinates
  double pt = 0;
  if (m_ptList.size() > 0) {
    int randIndex = m_flatGenerator() * m_ptList.size();
    pt = m_ptList[randIndex];
  } else {
    if (m_logSpacedPt) {
      double logPtMin = std::log10(m_minPt);
      double logPtMax = std::log10(m_maxPt);
      pt = pow(10, logPtMin + (logPtMax - logPtMin) * m_flatGenerator());

    } else {
      pt = m_minPt + m_flatGenerator() * (m_maxPt - m_minPt);
    }
  }
  px = pt * cos(phi);
  py = pt * sin(phi);
  pz = pt * sinh(eta);

  // randomly choose a particle type
  unsigned int currentType = (unsigned int)(m_pdgCodes.size() * m_flatGenerator());
  // protect against funnies
  if (currentType >= m_pdgCodes.size()) currentType = 0;

  momentum.SetPx(px);
  momentum.SetPy(py);
  momentum.SetPz(pz);
  momentum.SetE(std::sqrt(m_masses[currentType] * m_masses[currentType] + momentum.P2()));

  pdgId = m_pdgCodes[currentType];

  debug() << " -> " << pdgId << endmsg << "   P   = " << momentum << endmsg;
}

StatusCode ConstPtParticleGun::getNextEvent(HepMC::GenEvent& theEvent) {
  Gaudi::LorentzVector theFourMomentum;
  Gaudi::LorentzVector origin;
  // note: pgdid is set in function generateParticle
  int thePdgId;
  generateParticle(theFourMomentum, origin, thePdgId);

  // create HepMC Vertex --
  // by calling add_vertex(), the hepmc event is given ownership of the vertex
  HepMC::GenVertex* v = new HepMC::GenVertex(HepMC::FourVector(origin.X(), origin.Y(), origin.Z(), origin.T()));
  // create HepMC particle --
  // by calling add_particle_out(), the hepmc vertex is given ownership of the particle
  HepMC::GenParticle* p = new HepMC::GenParticle(
      HepMC::FourVector(theFourMomentum.Px(), theFourMomentum.Py(), theFourMomentum.Pz(), theFourMomentum.E()),
      thePdgId,
      1);  // hepmc status code for final state particle

  v->add_particle_out(p);

  theEvent.add_vertex(v);
  theEvent.set_signal_process_vertex(v);

  return StatusCode::SUCCESS;
}
