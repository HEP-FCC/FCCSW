#include "SimG4Fast/FastSimActions.h"

// FCCSW
#include "SimG4Fast/InitializeModelsRunAction.h"

namespace sim{
FastSimActions::FastSimActions(const std::string& aSmearingToolName, const std::string& aTrackerName):
  G4VUserActionInitialization(),
  m_smearToolName(aSmearingToolName),
  m_trackerName(aTrackerName) {}

FastSimActions::~FastSimActions() {}

void FastSimActions::Build() const {
  SetUserAction(new InitializeModelsRunAction(m_smearToolName, m_trackerName));
}
}
