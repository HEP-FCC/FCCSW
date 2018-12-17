
#ifndef SIMG4COMMON_CHARGINOPLUS_H
#define SIMG4COMMON_CHARGINOPLUS_H

#include "G4ParticleDefinition.hh"
#include "G4ios.hh"
#include "globals.hh"

/** @class sim::CharginoPlus SimG4Common/SimG4Common/CharginoPlus.h CharginoPlus.h
*
*  Chargino Plus particle definition for Geant4.
*  name of the particle: 's_chi_plus_1'.
*  pdg of the particle : 1000024
*  charge              : positive
*  Since the properties as e.g. mass, width, lifetime, of the Chargino are not known (it is a hypothetical particle)
* they need to be set at construction.
*
*  @author Julia Hrdinka
*
*/

namespace sim {
class CharginoPlus : public G4ParticleDefinition {
private:
  /// instance
  static CharginoPlus* s_theInstance;
  /// default constructor
  CharginoPlus() {}
  /// destructor
  ~CharginoPlus() {}

public:
  /// public definition method
  static CharginoPlus* Definition(G4double mass = -1, G4double width = -1, G4bool stable = true,
                                  G4double lifetime = -1);
  /// the pdg ID of the particle
  static int pdgID;
  /// the name of the particle
  static std::string name;
};
}
#endif  // SIMG4COMMON_CHARGINOPLUS_H
