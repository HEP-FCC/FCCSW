#include "SimG4Fast/FastSimActions.h"

// FCCSW
#include "SimG4Fast/SaveParticlesTrackingAction.h"
#include "SimG4Fast/InitializeModelsRunAction.h"

namespace sim{
FastSimActions::FastSimActions(const std::string& aSmearingToolName):
  G4VUserActionInitialization(),
  m_smearToolName(aSmearingToolName) {}

FastSimActions::~FastSimActions() {}

void FastSimActions::Build() const {
  SetUserAction(new SaveParticlesTrackingAction);
  SetUserAction(new InitializeModelsRunAction(m_smearToolName));
}
}
