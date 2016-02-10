#ifndef SIMG4FAST_FASTSIMACTIONS_H
#define SIMG4FAST_FASTSIMACTIONS_H

#include <string>

// Geant4
#include "G4VUserActionInitialization.hh"

/** @class FastSimActions SimG4Fast/SimG4Fast/FastSimActions.h FastSimActions.h
 *  User action initialization for fast simulation.
 *  @author    Anna Zaborowska
 */

namespace sim {
class FastSimActions : public G4VUserActionInitialization {
public:
  explicit FastSimActions(const std::string& aSmearingToolName);
  ~FastSimActions();
  /// Create all user actions.
  virtual void Build() const final;
private:
  /// Name of the smearing tool passed to InitializeModelsRunAction
  std::string m_smearToolName;
};
}

#endif /* SIMG4FAST_FASTSIMACTIONS_H */
