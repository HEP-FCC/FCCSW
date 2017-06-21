#include "SimG4Full/FullSimActions.h"

namespace sim {
FullSimActions::FullSimActions(bool enableHistory) : m_enableHistory(enableHistory), G4VUserActionInitialization() {}

FullSimActions::~FullSimActions() {}

void FullSimActions::Build() const {}

virtual SetUserAction(G4UserTrackingAction* outAction) const {
  if (m_enableHistory) {
    outAction = new ParticleHistoryAction;
  }
}
}
