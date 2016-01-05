#ifndef SIM_FASTSIMACTIONS_H
#define SIM_FASTSIMACTIONS_H

#include <string>

// Geant4
#include "G4VUserActionInitialization.hh"

/** @class FastSimActions FastSimActions.h SimG4Fast/FastSimActions.h
 *
   @brief     User action initialization.
   @author    Anna Zaborowska
*/

namespace sim {
class FastSimActions : public G4VUserActionInitialization {
public:
  FastSimActions(const std::string& aSmearingToolName);
  ~FastSimActions();
  /// A method where all  user actions are created.
  virtual void Build() const final;
private:
  /// Name of the smearing tool passed to run action that pairs the detector envelopes with fast sim models.
  std::string m_smearToolName;
};
}

#endif /* SIM_FASTSIMACTIONS_H */
