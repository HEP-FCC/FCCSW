#include "SimG4GflashSamplingCalo.h"

// Geant
#include "G4NistManager.hh"
#include "GFlashSamplingShowerParameterisation.hh"

DECLARE_TOOL_FACTORY(SimG4GflashSamplingCalo)

SimG4GflashSamplingCalo::SimG4GflashSamplingCalo(const std::string& type, const std::string& name, const IInterface* parent) :
GaudiTool(type, name, parent) {
  declareProperty("materials", m_materials);
  declareProperty("thickness", m_thickness);
}

SimG4GflashSamplingCalo::~SimG4GflashSamplingCalo() {}

StatusCode SimG4GflashSamplingCalo::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if(m_materials.size() != 2) {
    if(m_materials.size() == 0) {
      error()<<"Materials of the sampling calorimeter are not defined."<<endmsg;
    } else if(m_materials.size() == 1) {
      error()<<"For homogenous calorimeters use 'SimG4GflashHomoCalo tool."<<endmsg;
    } else {
      error()<<"One must specify two materials of the sampling calorimeter (active, passive)."<<endmsg;
    }
    return StatusCode::FAILURE;
  }
  G4NistManager* nist = G4NistManager::Instance();
  for(const auto& mat: m_materials) {
    if(nist->FindOrBuildMaterial(mat) == nullptr) {
      error()<<"Material <"<<mat<<"> is not found by G4NistManager."<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  if(m_thickness.size() != 2) {
    error()<<"One must specify thickness of both materials of the sampling calorimeter."<<endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4GflashSamplingCalo::finalize() {
  return GaudiTool::finalize();
}

std::unique_ptr<GVFlashShowerParameterisation> SimG4GflashSamplingCalo::parametrisation() {
  G4NistManager* nist = G4NistManager::Instance();
  std::unique_ptr<GVFlashShowerParameterisation> parametrisation = std::unique_ptr<GFlashSamplingShowerParameterisation>(new GFlashSamplingShowerParameterisation(
      nist->FindOrBuildMaterial(m_materials[0]),
      nist->FindOrBuildMaterial(m_materials[1]),
      m_thickness[0], m_thickness[1]));
  return std::move(parametrisation);
}
