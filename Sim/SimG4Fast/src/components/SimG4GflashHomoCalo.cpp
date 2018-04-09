#include "SimG4GflashHomoCalo.h"

// Geant
#include "G4NistManager.hh"
#include "GFlashHomoShowerParameterisation.hh"

DECLARE_TOOL_FACTORY(SimG4GflashHomoCalo)

SimG4GflashHomoCalo::SimG4GflashHomoCalo(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {}

SimG4GflashHomoCalo::~SimG4GflashHomoCalo() {}

StatusCode SimG4GflashHomoCalo::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (m_material.empty()) {
    error() << "Material of the calorimeter is not defined." << endmsg;
    return StatusCode::FAILURE;
  }
  G4NistManager* nist = G4NistManager::Instance();
  if (nist->FindOrBuildMaterial(m_material.value()) == nullptr) {
    error() << "Material <" << m_material << "> is not found by G4NistManager." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4GflashHomoCalo::finalize() { return GaudiTool::finalize(); }

std::unique_ptr<GVFlashShowerParameterisation> SimG4GflashHomoCalo::parametrisation() {
  G4NistManager* nist = G4NistManager::Instance();
  std::unique_ptr<GVFlashShowerParameterisation> parametrisation = std::unique_ptr<GFlashHomoShowerParameterisation>(
      new GFlashHomoShowerParameterisation(nist->FindOrBuildMaterial(m_material.value())));

  // TODO move to sampling tool
  // G4NistManager* nist = G4NistManager::Instance();
  // m_parametrisation = std::unique_ptr<GFlashSamplingShowerParameterisation>(new GFlashSamplingShowerParameterisation(
  //     nist->FindOrBuildMaterial("G4_lAr"),  nist->FindOrBuildMaterial("G4_Pb"), 6, 2));

  return std::move(parametrisation);
}
