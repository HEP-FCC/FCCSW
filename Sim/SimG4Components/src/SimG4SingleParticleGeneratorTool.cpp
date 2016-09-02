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
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

// datamodel
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"

// Declaration of the Tool
DECLARE_COMPONENT(SimG4SingleParticleGeneratorTool)

SimG4SingleParticleGeneratorTool::SimG4SingleParticleGeneratorTool(const std::string& type,
                                                             const std::string& nam,
                                                             const IInterface* parent)
    : GaudiTool(type, nam, parent) {
  declareInterface<ISimG4EventProviderTool>(this);
  declareProperty("particleName", m_particleName = "geantino", "Name of the generated particles");
  declareProperty("energyMin", m_energyMin = 1 * CLHEP::GeV, "Minimum energy of generated particles");
  declareProperty("energyMax", m_energyMax = 1 * CLHEP::TeV, "Maximum energy of generated particles");
  declareProperty("etaMin", m_etaMin = -5., "Minimum eta of generated particles");
  declareProperty("etaMax", m_etaMax = 5, "Maximum eta of generated particles");
  declareProperty("phiMin", m_phiMin = 0., "Minimum phi of generated particles");
  declareProperty("phiMax", m_phiMax = 2 * M_PI, "Maximum phi of generated particles");
  declareProperty("vertexX", m_vertexX = 0);
  declareProperty("vertexY", m_vertexY = 0);
  declareProperty("vertexZ", m_vertexZ = 0);
  declareProperty("saveEdm", m_saveEdm = false);
  declareOutput("genParticles", m_genParticlesHandle, "GenParticles");
  declareOutput("genVertices", m_genVerticesHandle, "GenVertices");
}

SimG4SingleParticleGeneratorTool::~SimG4SingleParticleGeneratorTool() {}

StatusCode SimG4SingleParticleGeneratorTool::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if(!G4ParticleTable::GetParticleTable()->contains(m_particleName)) {
    error()<<"Particle "<<m_particleName<<" cannot be found in G4ParticleTable"<<endmsg;
    return StatusCode::FAILURE;
  }
  if(m_energyMin > m_energyMax) {
    error()<<"Maximum energy cannot be lower than the minumum energy"<<endmsg;
    return StatusCode::FAILURE;
  }
  if(m_etaMin > m_etaMax) {
    error()<<"Maximum psudorapidity cannot be lower than the minumum psudorapidity"<<endmsg;
    return StatusCode::FAILURE;
  }
  if(m_phiMin > m_phiMax) {
    error()<<"Maximum azimuthal angle cannot be lower than the minumum angle"<<endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

G4Event* SimG4SingleParticleGeneratorTool::g4Event() {
  auto theEvent = new G4Event();
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDef = particleTable->FindParticle(m_particleName);
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
  auto& position = vertex.Position();
  position.X = aVertex->GetX0()*sim::g42edm::length;
  position.Y = aVertex->GetY0()*sim::g42edm::length;
  position.Z = aVertex->GetZ0()*sim::g42edm::length;
  vertex.Ctau(aVertex->GetT0()*Gaudi::Units::c_light*sim::g42edm::length);

  fcc::MCParticle particle = particles->create();
  fcc::BareParticle& core = particle.Core();
  core.Type = aParticle->GetPDGcode();
  core.Status = 1;
  core.P4.Px = aParticle->GetPx()*sim::g42edm::energy;
  core.P4.Py = aParticle->GetPy()*sim::g42edm::energy;
  core.P4.Pz = aParticle->GetPz()*sim::g42edm::energy;
  core.P4.Mass = aParticle->GetMass()*sim::g42edm::energy;
  particle.StartVertex(vertex);

  m_genParticlesHandle.put(particles);
  m_genVerticesHandle.put(vertices);
  return StatusCode::SUCCESS;
}
