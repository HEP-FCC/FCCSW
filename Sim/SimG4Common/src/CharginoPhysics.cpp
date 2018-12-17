#include "SimG4Common/CharginoPhysics.h"
#include "SimG4Common/CharginoMinus.h"
#include "SimG4Common/CharginoPlus.h"
#include "SimG4Common/Neutralino.h"

// Geant4
#include "G4DecayTable.hh"
#include "G4FastSimulationManagerProcess.hh"
#include "G4PhaseSpaceDecayChannel.hh"
#include "G4PhysicsConstructorFactory.hh"
#include "G4ProcessManager.hh"
#include "G4VDecayChannel.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"

namespace sim {

G4_DECLARE_PHYSCONSTR_FACTORY(CharginoPhysics);

CharginoPhysics::CharginoPhysics() : G4VPhysicsConstructor("CharginoPhysics") {}

CharginoPhysics::CharginoPhysics(ParticleConfiguration charginoPlus, ParticleConfiguration charginoMinus,
                                 ParticleConfiguration neutralino)
    : G4VPhysicsConstructor("CharginoPhysics"),
      m_charginoPlusMass(charginoPlus.mass),
      m_charginoPlusWidth(charginoPlus.width),
      m_charginoPlusStable(charginoPlus.stable),
      m_charginoPlusLifetime(charginoPlus.lifetime),
      m_charginoMinusMass(charginoMinus.mass),
      m_charginoMinusWidth(charginoMinus.width),
      m_charginoMinusStable(charginoMinus.stable),
      m_charginoMinusLifetime(charginoMinus.lifetime),
      m_neutralinoMass(neutralino.mass),
      m_neutralinoWidth(neutralino.width),
      m_neutralinoStable(neutralino.stable),
      m_neutralinoLifetime(neutralino.lifetime) {}

CharginoPhysics::~CharginoPhysics() {}

void CharginoPhysics::ConstructParticle() {
  // construct the particles
  std::cout << "ConstructParticle for CharginoPhysics" << std::endl;
  m_charginoPlus = sim::CharginoPlus::Definition(m_charginoPlusMass, m_charginoPlusWidth, m_charginoPlusStable,
                                                 m_charginoPlusLifetime);

  m_charginoMinus = sim::CharginoMinus::Definition(m_charginoMinusMass, m_charginoMinusWidth, m_charginoMinusStable,
                                                   m_charginoMinusLifetime);

  m_neutralino =
      sim::Neutralino::Definition(m_neutralinoMass, m_neutralinoWidth, m_neutralinoStable, m_neutralinoLifetime);
}

void CharginoPhysics::ConstructProcess() {
  // define physics
  std::cout << "ConstructProcess for CharginoPhysics" << std::endl;

  G4ProcessManager* charginoPlus = m_charginoPlus->GetProcessManager();
  G4ProcessManager* charginoMinus = m_charginoMinus->GetProcessManager();

  charginoPlus->AddProcess(new G4hMultipleScattering, -1, 1, 1);
  charginoMinus->AddProcess(new G4hMultipleScattering, -1, 1, 1);
  charginoPlus->AddProcess(new G4hIonisation, -1, 2, 2);
  charginoMinus->AddProcess(new G4hIonisation, -1, 2, 2);

  // decay
  G4DecayTable* charginoPlusTable = m_charginoPlus->GetDecayTable();
  if (!charginoPlusTable) {
    charginoPlusTable = new G4DecayTable();
  }
  G4VDecayChannel* charginoPlusChannel =
      new G4PhaseSpaceDecayChannel(m_charginoPlus->GetParticleName(), 1, 2, m_neutralino->GetParticleName(), "pi+");
  charginoPlusTable->Insert(charginoPlusChannel);
  std::cout << "Adding decay to pion and neutralino to " << m_charginoPlus->GetParticleName() << std::endl;
  m_charginoPlus->SetDecayTable(charginoPlusTable);
  //
  G4DecayTable* charginoMinusTable = m_charginoMinus->GetDecayTable();
  if (!charginoMinusTable) {
    charginoMinusTable = new G4DecayTable();
  }
  G4VDecayChannel* charginoMinusChannel =
      new G4PhaseSpaceDecayChannel(m_charginoMinus->GetParticleName(), 1, 2, m_neutralino->GetParticleName(), "pi-");
  charginoMinusTable->Insert(charginoMinusChannel);
  std::cout << "Adding decay to pion and neutralino to " << m_charginoMinus->GetParticleName() << std::endl;
  m_charginoMinus->SetDecayTable(charginoMinusTable);
}
}
