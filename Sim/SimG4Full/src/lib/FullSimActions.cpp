#include "SimG4Full/FullSimActions.h"
#include "SimG4Full/ParticleHistoryAction.h"
#include "SimG4Full/ParticleHistoryEventAction.h"
#include <iostream>

namespace sim {
FullSimActions::FullSimActions(bool enableHistory, double aEnergyCut, bool selectTaggedOnly)
    : G4VUserActionInitialization(),
      m_enableHistory(enableHistory),
      m_energyCut(aEnergyCut),
      m_selectTaggedOnly(selectTaggedOnly) {}

FullSimActions::~FullSimActions() {}

void FullSimActions::Build() const {
  if (m_enableHistory) {
    SetUserAction(new ParticleHistoryEventAction());
    SetUserAction(new ParticleHistoryAction(m_energyCut, m_selectTaggedOnly));
  }
}
}
