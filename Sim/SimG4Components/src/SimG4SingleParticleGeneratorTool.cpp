// local
#include "SimG4SingleParticleGeneratorTool.h"

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
DECLARE_COMPONENT(SimG4SingleParticleGeneratorTool)

SimG4SingleParticleGeneratorTool::SimG4SingleParticleGeneratorTool(const std::string& type,
                                                                   const std::string& nam,
                                                                   const IInterface* parent)
    : GaudiTool(type, nam, parent) {
  declareInterface<ISimG4EventProviderTool>(this);
  declareProperty("genParticles", m_genParticlesHandle, "Handle for the genparticles to be written");
  declareProperty("genVertices", m_genVerticesHandle, "Handle for the genvertices to be written");
}

SimG4SingleParticleGeneratorTool::~SimG4SingleParticleGeneratorTool() {}

StatusCode SimG4SingleParticleGeneratorTool::initialize() {
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

G4Event* SimG4SingleParticleGeneratorTool::g4Event() {
  auto theEvent = new G4Event();
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDef = particleTable->FindParticle(m_particleName.value());
  debug() << "particle definition " << particleDef << " +++ " << m_particleName << endmsg;
  G4double mass = particleDef->GetPDGMass();
  debug() << "particle mass = " << mass << endmsg;

  double particleEnergy = CLHEP::RandFlat::shoot(m_energyMin, m_energyMax);

  debug() << "particle energy = " << particleEnergy << endmsg;

  double eta = CLHEP::RandFlat::shoot(m_etaMin, m_etaMax);
  double phi = CLHEP::RandFlat::shoot(m_phiMin, m_phiMax);

  debug() << "particle eta, phi  = " << eta << " " << phi << endmsg;

  double theta = std::atan(std::exp(-eta)) * 2.;
  double randomX = std::sin(theta) * std::cos(phi);
  double randomY = std::sin(theta) * std::sin(phi);
  double randomZ = std::cos(theta);

  G4ThreeVector particleDir = G4ThreeVector(randomX, randomY, randomZ);
  G4ThreeVector particlePosition = G4ThreeVector(m_vertexX, m_vertexY, m_vertexZ);

  G4PrimaryVertex* vertex = new G4PrimaryVertex(particlePosition, 0.);
  G4PrimaryParticle* part = new G4PrimaryParticle(particleDef);

  part->SetMass(mass);
  part->SetKineticEnergy(particleEnergy);
  part->SetMomentumDirection(particleDir);
  part->SetCharge(particleDef->GetPDGCharge());

  vertex->SetPrimary(part);
  theEvent->AddPrimaryVertex(vertex);
  if (m_saveEdm) {
    saveToEdm(vertex, part);
  }
  return theEvent;
}

StatusCode SimG4SingleParticleGeneratorTool::saveToEdm(const G4PrimaryVertex* aVertex,
                                                       const G4PrimaryParticle* aParticle) {
  fcc::MCParticleCollection* particles = new fcc::MCParticleCollection();
  fcc::GenVertexCollection* vertices = new fcc::GenVertexCollection();
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
  core.p4.px = aParticle->GetPx() * sim::g42edm::energy;
  core.p4.py = aParticle->GetPy() * sim::g42edm::energy;
  core.p4.pz = aParticle->GetPz() * sim::g42edm::energy;
  core.p4.mass = aParticle->GetMass() * sim::g42edm::energy;
  particle.startVertex(vertex);

  m_genParticlesHandle.put(particles);
  m_genVerticesHandle.put(vertices);
  return StatusCode::SUCCESS;
}
