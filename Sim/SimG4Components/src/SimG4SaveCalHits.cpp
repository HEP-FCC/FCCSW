#include "SimG4SaveCalHits.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "SimG4Common/Units.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_TOOL_FACTORY(SimG4SaveCalHits)

SimG4SaveCalHits::SimG4SaveCalHits(const std::string& aType, const std::string& aName, const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("positionedCaloHits", m_positionedCaloHits,
                  "Handle for calo hits with additional position information");
  declareProperty("caloHits", m_caloHits, "Handle for calo hits");
}

SimG4SaveCalHits::~SimG4SaveCalHits() {}

StatusCode SimG4SaveCalHits::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  for (auto& readoutName : m_readoutNames) {
    if (allReadouts.find(readoutName) == allReadouts.end()) {
      error() << "Readout " << readoutName << " not found! Please check tool configuration." << endmsg;
      return StatusCode::FAILURE;
    } else {
      debug() << "Hits will be saved to EDM from the collection " << readoutName << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4SaveCalHits::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4SaveCalHits::saveOutput(const G4Event& aEvent) {
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  DD4hep::Simulation::Geant4CalorimeterHit* hit;
  if (collections != nullptr) {
    auto edmPositioned = m_positionedCaloHits.createAndPut();
    auto edmHits = m_caloHits.createAndPut();
    for (int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        size_t n_hit = collect->GetSize();
        debug() << "\t" << n_hit << " hits are stored in a collection #" << iter_coll << ": " << collect->GetName()
                << endmsg;
        for (size_t iter_hit = 0; iter_hit < n_hit; iter_hit++) {
          hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>(collect->GetHit(iter_hit));
          auto edmHit = edmHits->create();
          auto& edmHitCore = edmHit.core();
          edmHitCore.cellId = hit->cellID;
          edmHitCore.energy = hit->energyDeposit * sim::g42edm::energy;
          auto position = fcc::Point();
          position.x = hit->position.x() * sim::g42edm::length;
          position.y = hit->position.y() * sim::g42edm::length;
          position.z = hit->position.z() * sim::g42edm::length;
          auto posHit = edmPositioned->create(position, edmHitCore);
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}
