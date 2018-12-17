#ifndef SIMG4COMMON_CHARGINOMINUS_H
#define SIMG4COMMON_CHARGINOMINUS_H

#include "G4ParticleDefinition.hh"
#include "G4ios.hh"
#include "globals.hh"

/** @class sim::CharginoMinus SimG4Common/SimG4Common/CharginoMinus.h CharginoMinus.h
*
*  Chargino Minus particle definition for Geant4.
*  name of the particle: 's_chi_minus_1'.
*  pdg of the particle : -1000024
*  charge              : negative
*  Since the properties as e.g. mass, width, lifetime, of the Chargino are not known (it is a hypothetical particle)
* they need to be set at construction.
*
*  @author Julia Hrdinka
*
*/

namespace sim {
class CharginoMinus : public G4ParticleDefinition {
private:
  /// instance
  static CharginoMinus* s_theInstance;
  /// default constructor
  CharginoMinus() {}
  /// destructor
  ~CharginoMinus() {}

public:
  /// public definition method
  static CharginoMinus* Definition(G4double mass = -1, G4double width = -1, G4bool stable = true,
                                   G4double lifetime = -1);
  /// the pdg ID of the particle
  static int pdgID;
  /// the name of the particle
  static std::string name;
};
}
#endif  // SIMG4COMMON_CHARGINOMINUS_H
