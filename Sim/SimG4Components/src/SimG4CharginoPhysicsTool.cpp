// local
#include "SimG4CharginoPhysicsTool.h"

// FCCSW
#include "SimG4Common/CharginoMinus.h"
#include "SimG4Common/CharginoPhysics.h"
#include "SimG4Common/CharginoPlus.h"
#include "SimG4Common/Neutralino.h"
#include "SimG4Common/Units.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// CLHEP
#include <CLHEP/Random/RandFlat.h>

// Geant4
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4VModularPhysicsList.hh"

// datamodel
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

// Declaration of the Tool
DECLARE_TOOL_FACTORY(SimG4CharginoPhysicsTool)

SimG4CharginoPhysicsTool::SimG4CharginoPhysicsTool(const std::string& aType, const std::string& aName,
                                                   const IInterface* aParent)
    : AlgTool(aType, aName, aParent) {
  declareInterface<ISimG4PhysicsList>(this);
  declareProperty("fullphysics", m_physicsListTool, "Handle for the full physics list tool");
}

SimG4CharginoPhysicsTool::~SimG4CharginoPhysicsTool() {}

StatusCode SimG4CharginoPhysicsTool::initialize() {
  if (AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_physicsListTool.retrieve();
  return StatusCode::SUCCESS;
}

StatusCode SimG4CharginoPhysicsTool::finalize() { return AlgTool::finalize(); }

G4VModularPhysicsList* SimG4CharginoPhysicsTool::physicsList() {
  debug() << "Construct physics for Charginos and Neutralinos" << endmsg;

  debug() << "     charginoPlus mass  : " << m_charginoPlusMass << ", lifetime: " << m_charginoPlusLifetime << endmsg;
  debug() << "     charginoMinus mass : " << m_charginoMinusMass << ", lifetime: " << m_charginoMinusLifetime << endmsg;
  debug() << "     neutralino mass    : " << m_neutralinoMass << ", lifetime: " << m_neutralinoMass << endmsg;
  // Configure Physics
  sim::CharginoPhysics::ParticleConfiguration charginoPlusConfig;
  charginoPlusConfig.mass = m_charginoPlusMass;
  charginoPlusConfig.lifetime = m_charginoPlusLifetime;
  charginoPlusConfig.stable = true;
  charginoPlusConfig.width = 0.;
  // in case particle is not stable
  if (m_charginoPlusLifetime > 0.) {
    charginoPlusConfig.stable = false;
    charginoPlusConfig.width = CLHEP::hbar_Planck / m_charginoPlusLifetime;
  }

  sim::CharginoPhysics::ParticleConfiguration charginoMinusConfig;
  charginoMinusConfig.mass = m_charginoMinusMass;
  charginoMinusConfig.lifetime = m_charginoMinusLifetime;
  charginoMinusConfig.stable = true;
  charginoMinusConfig.width = 0.;
  if (m_charginoMinusLifetime > 0.) {
    charginoMinusConfig.stable = false;
    charginoMinusConfig.width = CLHEP::hbar_Planck / m_charginoMinusLifetime;
  }

  sim::CharginoPhysics::ParticleConfiguration neutralinoConfig;
  neutralinoConfig.mass = m_neutralinoMass;
  neutralinoConfig.lifetime = m_neutralinoLifetime;
  neutralinoConfig.stable = true;
  neutralinoConfig.width = 0.;
  if (m_neutralinoLifetime > 0.) {
    neutralinoConfig.stable = false;
    neutralinoConfig.width = CLHEP::hbar_Planck / m_neutralinoLifetime;
  }

  // ownership passed to SimG4Svc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  G4VModularPhysicsList* physicsList = m_physicsListTool->physicsList();
  // register chargino physics
  physicsList->RegisterPhysics(new sim::CharginoPhysics(charginoPlusConfig, charginoMinusConfig, neutralinoConfig));
  // hand back physics list
  return physicsList;
}
