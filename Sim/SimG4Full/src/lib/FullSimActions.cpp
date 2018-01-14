#include "SimG4Full/FullSimActions.h"
#include "SimG4Full/ParticleHistoryAction.h"
#include <iostream>

namespace sim {
FullSimActions::FullSimActions(bool enableHistory) : G4VUserActionInitialization(), m_enableHistory(enableHistory) {}

FullSimActions::~FullSimActions() {}

void FullSimActions::Build() const {
  if (m_enableHistory) {
    SetUserAction(new ParticleHistoryAction());
  }
}
}
