#include "InspectHitsCollectionsTool.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// Geant
#include "G4Event.hh"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_TOOL_FACTORY(InspectHitsCollectionsTool)

InspectHitsCollectionsTool::InspectHitsCollectionsTool(const std::string& aType, const std::string& aName,
                                                       const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
}

InspectHitsCollectionsTool::~InspectHitsCollectionsTool() {}

StatusCode InspectHitsCollectionsTool::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
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

StatusCode InspectHitsCollectionsTool::finalize() { return GaudiTool::finalize(); }

StatusCode InspectHitsCollectionsTool::saveOutput(const G4Event& aEvent) {
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  DD4hep::Simulation::Geant4TrackerHit* hitT;
  DD4hep::Simulation::Geant4CalorimeterHit* hitC;
  info() << "Obtaining hits collections that are stored in this event:" << endmsg;
  if (collections != nullptr) {
    for (int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        info() << "\tcollection #: " << iter_coll << "\tname: " << collect->GetName()
               << "\tsize: " << collect->GetSize() << endmsg;
        size_t n_hit = collect->GetSize();
        auto decoder = m_geoSvc->lcdd()->readout(collect->GetName()).idSpec().decoder();
        for (size_t iter_hit = 0; iter_hit < n_hit; iter_hit++) {
          hitT = dynamic_cast<DD4hep::Simulation::Geant4TrackerHit*>(collect->GetHit(iter_hit));
          if (hitT) {
            decoder->setValue(hitT->cellID);
            debug() << "hit Edep: " << hitT->energyDeposit << "\tcellID: " << hitT->cellID << "\t"
                    << decoder->valueString() << endmsg;
          } else {
            hitC = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>(collect->GetHit(iter_hit));
            if (hitC) {
              decoder->setValue(hitC->cellID);
              debug() << "hit Edep: " << hitC->energyDeposit << "\tcellID: " << hitC->cellID << "\t"
                      << decoder->valueString() << endmsg;
            }
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}
