#include "SimG4Full/ParticleHistoryAction.h"

namespace sim {

ParticleHistoryAction::ParticleHistoryAction(ParticleHistoryManager& aHistoryMgr) : m_historyMgr(aHistoryMgr) {}

void ParticleHistoryAction::PreUserTrackingAction(const G4Track*) {}
void ParticleHistoryAction::PostUserTrackingAction(const G4Track*) {}
}
