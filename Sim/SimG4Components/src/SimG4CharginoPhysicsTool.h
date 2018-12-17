#ifndef SIMG4COMPONENTS_G4CHARGINOPHYSICSTOOL_H
#define SIMG4COMPONENTS_G4CHARGINOPHYSICSTOOL_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/ISimG4PhysicsList.h"

// Geant4
#include "G4SystemOfUnits.hh"
#include "G4VPhysicsConstructor.hh"

// Forward declarations

/** @class SimG4CharginoPhysicsTool SimG4CharginoPhysicsTool.h "SimG4CharginoPhysicsTool.h"
*
*  Tool that configures the physics list for charginos and neutralinos. Since the parameters of these hypothetical
particles are not known, they are made configurable.
*
*  @author Julia Hrdinka
*
*/

class SimG4CharginoPhysicsTool : public AlgTool, virtual public ISimG4PhysicsList {
public:
  /// Standard constructor
  SimG4CharginoPhysicsTool(const std::string& type, const std::string& name, const IInterface* parent);
  /// initialize gaudi interface method
  virtual StatusCode initialize() final;
  /// finalize gaudi interface method
  virtual StatusCode finalize();
  /// Get the physics list.
  ///  @return pointer to G4VModularPhysicsList (ownership is transferred to the caller)
  virtual G4VModularPhysicsList* physicsList();
  /// Destructor
  virtual ~SimG4CharginoPhysicsTool();

private:
  /// Handle for the full physics list tool
  ToolHandle<ISimG4PhysicsList> m_physicsListTool{"SimG4FtfpBert", this, true};
  /// The mass of the chargino - parameter can be configured by hand in job options
  Gaudi::Property<double> m_charginoPlusMass{this, "CharginoPlusMass", 101.0 * CLHEP::GeV, "CharginoMass width"};
  /// The life time of the chargino - parameter can be configured by hand in job options
  Gaudi::Property<double> m_charginoPlusLifetime{this, "CharginoPlusLifetime", -1, "CharginoPlus life time"};

  /// The mass of the chargino - parameter can be configured by hand in job options
  Gaudi::Property<double> m_charginoMinusMass{this, "CharginoMinusMass", 101.0 * CLHEP::GeV, "CharginoMass width"};
  /// The life time of the chargino - parameter can be configured by hand in job options
  Gaudi::Property<double> m_charginoMinusLifetime{this, "CharginoMinusLifetime", -1, "CharginoMinus life time"};

  /// The mass of the neutralino - parameter can be configured by hand in job options
  Gaudi::Property<double> m_neutralinoMass{this, "NeutralinoMass", 101.0 * CLHEP::GeV, "NeutralinoMass width"};
  /// The life time of the neutralino - parameter can be configured by hand in job options
  Gaudi::Property<double> m_neutralinoLifetime{this, "NeutralinoLifetime", -1, "Neutralino life time"};
  /// the chargino plus
  G4ParticleDefinition* m_charginoPlus;
  /// the chargino minus
  G4ParticleDefinition* m_charginoMinus;
  /// the neutralino
  G4ParticleDefinition* m_neutralino;
};

#endif  // SIMG4COMPONENTS_G4CHARGINOPHYSICSTOOL_H
