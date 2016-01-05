#include "G4FastSimActions.h"

// FCCSW
#include "SimG4Fast/FastSimActions.h"
#include "SimG4Interface/ISmearingTool.h"

DECLARE_COMPONENT(G4FastSimActions)

G4FastSimActions::G4FastSimActions(const std::string& type, const std::string& name, const IInterface* parent) :
GaudiTool(type, name, parent) {
  declareInterface<IG4ActionTool>(this);
  declareProperty ("smearing", m_smearToolName = "SimpleSmear");
}

G4FastSimActions::~G4FastSimActions() {}

StatusCode G4FastSimActions::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_smearTool = tool<ISmearingTool>(m_smearToolName);
  return StatusCode::SUCCESS;
}

G4VUserActionInitialization* G4FastSimActions::getUserActionInitialization() {
  return new sim::FastSimActions(m_smearToolName);
}
