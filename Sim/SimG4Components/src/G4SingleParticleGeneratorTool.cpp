
// Include files

// local
#include "G4SingleParticleGeneratorTool.h"

// CLHEP
#include <CLHEP/Random/RandFlat.h>

// Geant4
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

// Declaration of the Tool
DECLARE_COMPONENT( G4SingleParticleGeneratorTool )

G4SingleParticleGeneratorTool::G4SingleParticleGeneratorTool( const std::string& type,
                                                              const std::string& nam,
                                                              const IInterface* parent )
  : GaudiTool ( type, nam, parent )
{
  declareInterface< IG4EventGeneratorTool >( this ) ;
  declareProperty("particleName", m_particleName = "geantino", "Name of the generated particles");
  declareProperty("energyMin", m_energyMin = 1 * CLHEP::GeV, "Minimum energy of generated particles");
  declareProperty("energyMax", m_energyMax = 1 * CLHEP::TeV, "Maximum energy of generated particles");
  declareProperty("etaMin", m_etaMin = -5., "Minimum eta of generated particles");
  declareProperty("etaMax", m_etaMax = 5, "Maximum eta of generated particles");
  declareProperty("phiMin", m_phiMin = 0., "Minimum phi of generated particles");
  declareProperty("phiMax", m_phiMax = 2*M_PI, "Maximum phi of generated particles");
  declareProperty("vertexX", m_vertexX = 0);
  declareProperty("vertexY", m_vertexY = 0);
  declareProperty("vertexZ", m_vertexZ = 0);
}

G4SingleParticleGeneratorTool::~G4SingleParticleGeneratorTool()
{
}

StatusCode G4SingleParticleGeneratorTool::initialize( )
{
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  return sc;
}


StatusCode G4SingleParticleGeneratorTool::finalize( )
{
  return StatusCode::SUCCESS;
}

G4Event* G4SingleParticleGeneratorTool::g4Event()
{
  auto theEvent = new G4Event();
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDef = particleTable->FindParticle(m_particleName);
  debug() << " particle definition " << particleDef << " +++ " << m_particleName << endmsg;
  G4double mass = 0;
  if (m_particleName != "geantino") {
    mass = particleDef->GetPDGMass();
  }
  debug() << "   particle mass = " << mass << endmsg;

  double particleEnergy = CLHEP::RandFlat::shoot(m_energyMin, m_energyMax);

  debug() << "    particle energy = " << particleEnergy << endmsg;

  double eta = CLHEP::RandFlat::shoot(m_etaMin, m_etaMax);
  double phi = CLHEP::RandFlat::shoot(m_phiMin, m_phiMax);

  debug() << "    particle eta, phi  = " << eta << " " << phi << endmsg;

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

  return theEvent;
}
