#include "SimG4Full/FullSimActions.h"
#include "SimG4Full/ParticleHistoryAction.h"
#include <iostream>

namespace sim {
FullSimActions::FullSimActions(bool enableHistory, double energyCut) : G4VUserActionInitialization(), m_enableHistory(enableHistory), m_energyCut(energyCut) {}

FullSimActions::~FullSimActions() {}

void FullSimActions::Build() const {
  if (m_enableHistory) {
    SetUserAction(new ParticleHistoryAction(m_energyCut));
  }
}
}
