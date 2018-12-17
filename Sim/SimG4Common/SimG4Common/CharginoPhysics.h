#ifndef SIMG4COMMON_CHARGINOPHYSICS_H
#define SIMG4COMMON_CHARGINOPHYSICS_H

// Geant
#include "G4VPhysicsConstructor.hh"

/** @class CharginoPhysics SimG4Common/SimG4Common/CharginoPhysics.h CharginoPhysics.h
 *
 *  Physics constructor for simple chargino physics.
 *  It defines the chargino plus/minus and the neutralino.
 *  Ionisation and multiple scattering is defined for the charginos as well as their decay into a neutralino and a pion.
 *
 *  @author Julia Hrdinka
 */

namespace sim {
class CharginoPhysics : public G4VPhysicsConstructor {
public:
  /// helper struct to configure charginos and neutralinos
  struct ParticleConfiguration {
    double mass = 101.0 * CLHEP::GeV;
    double width = 0.0 * CLHEP::MeV;
    bool stable = true;
    double lifetime = -1;
  };
  /// default constructor.
  CharginoPhysics();
  /// Constructor.
  CharginoPhysics(ParticleConfiguration charginoPlus, ParticleConfiguration charginoMinus,
                  ParticleConfiguration neutralino);
  /// Destructor.
  virtual ~CharginoPhysics();
  /// Add the process of parametrisation to every existing particle
  ///(created by the G4ModularPhysicsList to which it is registered)
  virtual void ConstructProcess() final;
  /// Construction of particles. Nothing to be done by fast sim (parametrisation).
  virtual void ConstructParticle() final;

private:
  /// chargino mass
  double m_charginoPlusMass = 101.0 * CLHEP::GeV;
  /// The width of the chargino
  double m_charginoPlusWidth = 0.0 * CLHEP::MeV;
  /// Flag indicating if the chargino is stable
  bool m_charginoPlusStable = true;
  /// The life time of the chargino
  double m_charginoPlusLifetime = -1;
  /// the chargino plus
  G4ParticleDefinition* m_charginoPlus;

  /// chargino mass
  double m_charginoMinusMass = 101.0 * CLHEP::GeV;
  /// The width of the chargino
  double m_charginoMinusWidth = 0.0 * CLHEP::MeV;
  /// Flag indicating if the chargino is stable
  bool m_charginoMinusStable = true;
  /// The life time of the chargino
  double m_charginoMinusLifetime = -1;
  /// the chargino minus
  G4ParticleDefinition* m_charginoMinus;

  /// neutralino mass
  double m_neutralinoMass = 101.0 * CLHEP::GeV;
  /// The width of the neutralino
  double m_neutralinoWidth = 0.0 * CLHEP::MeV;
  /// Flag indicating if the neutralino is stable
  bool m_neutralinoStable = true;
  /// The life time of the neutralino
  double m_neutralinoLifetime = -1;
  /// the neutralino
  G4ParticleDefinition* m_neutralino;
};
}

#endif  // SIMG4COMMON_CHARGINOPHYSICS_H
