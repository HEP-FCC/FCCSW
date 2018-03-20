// local
#include "SimG4SingleParticleGeneratorToolPt.h"

// FCCSW
#include "SimG4Common/Units.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// CLHEP
#include <CLHEP/Random/RandFlat.h>

// Geant4
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

// datamodel
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

// Declaration of the Tool
DECLARE_COMPONENT(SimG4SingleParticleGeneratorToolPt)

SimG4SingleParticleGeneratorToolPt::SimG4SingleParticleGeneratorToolPt(const std::string& type,
                                                                   const std::string& nam,
                                                                   const IInterface* parent)
    : GaudiTool(type, nam, parent) {
  declareInterface<ISimG4EventProviderTool>(this);
  declareProperty("genParticles", m_genParticlesHandle, "Handle for the genparticles to be written");
  declareProperty("genVertices", m_genVerticesHandle, "Handle for the genvertices to be written");
}

SimG4SingleParticleGeneratorToolPt::~SimG4SingleParticleGeneratorToolPt() {}

StatusCode SimG4SingleParticleGeneratorToolPt::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (!G4ParticleTable::GetParticleTable()->contains(m_particleName.value())) {
    error() << "Particle " << m_particleName << " cannot be found in G4ParticleTable" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_energyMin > m_energyMax) {
    error() << "Maximum energy cannot be lower than the minumum energy" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_etaMin > m_etaMax) {
    error() << "Maximum psudorapidity cannot be lower than the minumum psudorapidity" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_phiMin > m_phiMax) {
    error() << "Maximum azimuthal angle cannot be lower than the minumum angle" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

G4Event* SimG4SingleParticleGeneratorToolPt::g4Event() {
  auto theEvent = new G4Event();
  std::vector<G4PrimaryVertex*> vv;
  std::vector<G4PrimaryParticle*> pp;
      int randIndexnTracks = CLHEP::RandFlat::shoot(0.,1.) * (m_nTracksList.size());
      unsigned int nTracks = m_nTracksList[randIndexnTracks];
  for (int i_pileup = 0; i_pileup < nTracks; ++i_pileup) {
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particleDef = particleTable->FindParticle(m_particleName.value());
    debug() << "particle definition " << particleDef << " +++ " << m_particleName << endmsg;
    G4double mass = particleDef->GetPDGMass();
    debug() << "particle mass = " << mass << endmsg;

   
    double particleEnergy = CLHEP::RandFlat::shoot(m_energyMin, m_energyMax);


    double eta = CLHEP::RandFlat::shoot(m_etaMin, m_etaMax);
    double phi = CLHEP::RandFlat::shoot(m_phiMin, m_phiMax);
    if (m_ptList.size() > 0 ) {
      int randIndex = CLHEP::RandFlat::shoot(0.,1.) * (m_ptList.size());
      particleEnergy = m_ptList[randIndex];

    }
    debug() << "particle energy = " << particleEnergy << endmsg;

    debug() << "particle eta, phi  = " << eta << " " << phi << endmsg;

    double randomX = particleEnergy  * std::cos(phi);
    double randomY = particleEnergy  * std::sin(phi);
    double randomZ = std::sinh(eta) * particleEnergy;

    G4ThreeVector particleDir = G4ThreeVector(randomX, randomY, randomZ);
    G4ThreeVector particlePosition = G4ThreeVector(m_vertexX, m_vertexY, m_vertexZ);

    G4PrimaryVertex* vertex = new G4PrimaryVertex(particlePosition, 0.);
    G4PrimaryParticle* part = new G4PrimaryParticle(particleDef);


    part->SetMass(mass);
    part->SetKineticEnergy(particleEnergy);
    part->SetMomentum(randomX, randomY, randomZ);
    if (CLHEP::RandFlat::shoot(0.,1.) < 0.5) {
      part->SetCharge(+1.);
    } else { 
      part->SetCharge(-1.);
  }

    vertex->SetPrimary(part);
    theEvent->AddPrimaryVertex(vertex);
    vv.push_back(vertex);
    pp.push_back(part);
  }
  if (m_saveEdm) {
    saveToEdm(vv, pp);
  }
  return theEvent;
}

StatusCode SimG4SingleParticleGeneratorToolPt::saveToEdm(std::vector<G4PrimaryVertex*> aVertexs,
                                                       std::vector<G4PrimaryParticle*> aParticles) {
  fcc::MCParticleCollection* particles = new fcc::MCParticleCollection();
  fcc::GenVertexCollection* vertices = new fcc::GenVertexCollection();
  for (int i = 0; i < aVertexs.size(); i++) {
    auto aVertex = aVertexs[i];
    auto aParticle = aParticles[i];
  auto vertex = vertices->create();
  auto& position = vertex.position();
  position.x = aVertex->GetX0() * sim::g42edm::length;
  position.y = aVertex->GetY0() * sim::g42edm::length;
  position.z = aVertex->GetZ0() * sim::g42edm::length;
  vertex.ctau(aVertex->GetT0() * Gaudi::Units::c_light * sim::g42edm::length);

  fcc::MCParticle particle = particles->create();
  fcc::BareParticle& core = particle.core();
  core.pdgId = aParticle->GetPDGcode();
  core.status = 1;
  core.bits = i;
  core.p4.px = aParticle->GetPx() * sim::g42edm::energy;
  core.p4.py = aParticle->GetPy() * sim::g42edm::energy;
  core.p4.pz = aParticle->GetPz() * sim::g42edm::energy;
  core.p4.mass = aParticle->GetMass() * sim::g42edm::energy;
  particle.startVertex(vertex);
  }

  m_genParticlesHandle.put(particles);
  m_genVerticesHandle.put(vertices);
  return StatusCode::SUCCESS;
}
