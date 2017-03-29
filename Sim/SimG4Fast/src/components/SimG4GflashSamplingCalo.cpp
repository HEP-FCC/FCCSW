#include "SimG4GflashSamplingCalo.h"

// Geant
#include "G4NistManager.hh"
#include "GFlashSamplingShowerParameterisation.hh"

DECLARE_TOOL_FACTORY(SimG4GflashSamplingCalo)

SimG4GflashSamplingCalo::SimG4GflashSamplingCalo(const std::string& type, const std::string& name,
                                                 const IInterface* parent)
    : GaudiTool(type, name, parent) {}

SimG4GflashSamplingCalo::~SimG4GflashSamplingCalo() {}

StatusCode SimG4GflashSamplingCalo::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (m_thicknessActive == 0 || m_thicknessPassive == 0) {
    error() << "Both layers of the detector should have the thickness specified." << endmsg;
    return StatusCode::FAILURE;
  }
  G4NistManager* nist = G4NistManager::Instance();
  if (nist->FindOrBuildMaterial(m_materialActive.value()) == nullptr) {
    error() << "Material <" << m_materialActive << "> is not found by G4NistManager." << endmsg;
    return StatusCode::FAILURE;
  }
  if (nist->FindOrBuildMaterial(m_materialPassive.value()) == nullptr) {
    error() << "Material <" << m_materialPassive << "> is not found by G4NistManager." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4GflashSamplingCalo::finalize() { return GaudiTool::finalize(); }

std::unique_ptr<GVFlashShowerParameterisation> SimG4GflashSamplingCalo::parametrisation() {
  G4NistManager* nist = G4NistManager::Instance();
  std::unique_ptr<GVFlashShowerParameterisation> parametrisation =
      std::unique_ptr<GFlashSamplingShowerParameterisation>(new GFlashSamplingShowerParameterisation(
          nist->FindOrBuildMaterial(m_materialActive.value()), nist->FindOrBuildMaterial(m_materialPassive.value()),
          m_thicknessActive.value(), m_thicknessPassive.value()));
  return std::move(parametrisation);
}
