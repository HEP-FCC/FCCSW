#include "SimG4Fast/FastSimActions.h"

// FCCSW
#include "SimG4Fast/InitializeModelsRunAction.h"

namespace sim{
FastSimActions::FastSimActions(const std::string& aSmearingToolName):
  G4VUserActionInitialization(),
  m_smearToolName(aSmearingToolName) {}

FastSimActions::~FastSimActions() {}

void FastSimActions::Build() const {
  SetUserAction(new InitializeModelsRunAction(m_smearToolName));
}
}
