#include "SimG4FastSimCalorimeterRegion.h"

// Geant4
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4RegionStore.hh"
#include "G4TransportationManager.hh"
#include "G4VFastSimulationModel.hh"
#include "GFlashShowerModel.hh"

#include "G4NistManager.hh"

DECLARE_TOOL_FACTORY(SimG4FastSimCalorimeterRegion)

SimG4FastSimCalorimeterRegion::SimG4FastSimCalorimeterRegion(const std::string& type, const std::string& name,
                                                             const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ISimG4RegionTool>(this);
  declareProperty("parametrisation", m_parametrisationTool,
                  "Pointer to a parametrisation tool, to retrieve calorimeter parametrisation");
}

SimG4FastSimCalorimeterRegion::~SimG4FastSimCalorimeterRegion() {}

StatusCode SimG4FastSimCalorimeterRegion::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (m_volumeNames.size() == 0) {
    error() << "No detector name is specified for the parametrisation" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_minTriggerEnergy > m_maxTriggerEnergy) {
    error() << "Energy range is not defined properly" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_parametrisationTool.retrieve()) {
    error() << "GFlash parametrisation tool cannot be retieved" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4FastSimCalorimeterRegion::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4FastSimCalorimeterRegion::create() {
  G4LogicalVolume* world =
      (*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume();
  for (const auto& calorimeterName : m_volumeNames) {
    for (int iter_region = 0; iter_region < world->GetNoDaughters(); ++iter_region) {
      if (world->GetDaughter(iter_region)->GetName().find(calorimeterName) != std::string::npos) {
        /// all G4Region objects are deleted by the G4RegionStore
        m_g4regions.emplace_back(
            new G4Region(world->GetDaughter(iter_region)->GetLogicalVolume()->GetName() + "_fastsim"));
        m_g4regions.back()->AddRootLogicalVolume(world->GetDaughter(iter_region)->GetLogicalVolume());
        std::unique_ptr<GFlashShowerModel> model(
            new GFlashShowerModel(m_g4regions.back()->GetName(), m_g4regions.back()));
        // make model active (by default it is inactive)
        model->SetFlagParamType(1);
        // energy cuts - currently all are the same
        m_particleBounds = std::unique_ptr<GFlashParticleBounds>(new GFlashParticleBounds());
        m_particleBounds->SetMinEneToParametrise(*G4Electron::ElectronDefinition(),
                                                 m_minTriggerEnergy / Gaudi::Units::MeV);
        m_particleBounds->SetMinEneToParametrise(*G4Positron::PositronDefinition(),
                                                 m_minTriggerEnergy / Gaudi::Units::MeV);
        m_particleBounds->SetMaxEneToParametrise(*G4Electron::ElectronDefinition(),
                                                 m_maxTriggerEnergy / Gaudi::Units::MeV);
        m_particleBounds->SetMaxEneToParametrise(*G4Positron::PositronDefinition(),
                                                 m_maxTriggerEnergy / Gaudi::Units::MeV);
        m_particleBounds->SetEneToKill(*G4Electron::ElectronDefinition(), m_energyToKill / Gaudi::Units::MeV);
        m_particleBounds->SetEneToKill(*G4Positron::PositronDefinition(), m_energyToKill / Gaudi::Units::MeV);
        model->SetParticleBounds(*m_particleBounds);

        // set parametrisation with the material
        m_parametrisation = std::unique_ptr<GVFlashShowerParameterisation>(m_parametrisationTool->parametrisation());
        model->SetParameterisation(*m_parametrisation);
        // Makes the Energy Spots in the SD attached to the volume
        m_hitMaker = std::unique_ptr<GFlashHitMaker>(new GFlashHitMaker());
        model->SetHitMaker(*m_hitMaker);
        m_models.push_back(std::move(model));
        info() << "Attaching a Calorimeter fast simulation model (GFlash) to the region "
               << m_g4regions.back()->GetName() << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}
