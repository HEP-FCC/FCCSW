#ifndef SIMG4COMMON_NEUTRALINO_H
#define SIMG4COMMON_NEUTRALINO_H

#include "G4ParticleDefinition.hh"
#include "G4ios.hh"
#include "globals.hh"

/** @class sim::Neutralino SimG4Common/SimG4Common/Neutralino.h Neutralino.h
*
*  Neutralino particle definition for Geant4.
*  name of the particle: 's_chi_0_1'
*  pdg of the particle : 1000022
*  charge              : neutral
*  Since the properties as e.g. mass, width, lifetime, of the Neutralino are not known (it is a hypothetical particle)
* they need to be set at construction.
*
*  @author Julia Hrdinka
*
*/

namespace sim {

class Neutralino : public G4ParticleDefinition {
private:
  /// instance
  static Neutralino* s_theInstance;
  /// default constructor
  Neutralino() {}
  /// destructor
  ~Neutralino() {}

public:
  static Neutralino* Definition(G4double mass = -1, G4double width = -1, G4bool stable = true, G4double lifetime = -1);

  /// the pdg ID of the particle
  static int pdgID;
  /// the name of the particle
  static std::string name;
};
}
#endif  // SIMG4COMMON_NEUTRALINO_H
