#include "SimG4SaveTestCalHits.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "TestGeometry/TestCalorimeterHit.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

DECLARE_TOOL_FACTORY(SimG4SaveTestCalHits)

SimG4SaveTestCalHits::SimG4SaveTestCalHits(const std::string& aType, const std::string& aName,
                                           const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("caloClusters", m_caloHitsPositioned, "Handle for calo clusters");
  declareProperty("caloHits", m_caloHits, "Handle for calo hits");
}

SimG4SaveTestCalHits::~SimG4SaveTestCalHits() {}

StatusCode SimG4SaveTestCalHits::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (m_calType.value().find("ECal") != std::string::npos && m_calType.value().find("HCal") != std::string::npos) {
    error() << "Wrong type of the calorimeter. Set the property 'caloType' to either 'ECal' or 'HCal'" << endmsg;
    return StatusCode::FAILURE;
  } else {
    info() << "Initializing a tool saving the outputs for the calorimeter type: " << m_calType << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4SaveTestCalHits::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4SaveTestCalHits::saveOutput(const G4Event& aEvent) {
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  test::TestCalorimeterHit* hit;
  double fCellNo = 11.;
  double energyTotal;
  int hitNo;
  if (collections != nullptr) {
    auto edmPositioned = m_caloHitsPositioned.createAndPut();
    auto edmHits = m_caloHits.createAndPut();
    ;
    for (int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (collect->GetName().find(m_calType.value()) != std::string::npos) {
        size_t n_hit = collect->GetSize();
        energyTotal = 0;
        hitNo = 0;
        info() << "\t" << n_hit << " hits are stored in a calorimeter collection #" << iter_coll << ": "
               << collect->GetName() << endmsg;
        for (size_t iter_hit = 0; iter_hit < n_hit; iter_hit++) {
          hit = dynamic_cast<test::TestCalorimeterHit*>(collect->GetHit(iter_hit));
          if (hit->GetXid() != -1 && hit->GetYid() != -1 && hit->GetZid() != -1) {
            auto edmHit = edmHits->create();
            auto& edmHitCore = edmHit.core();
            edmHitCore.cellId = fCellNo * fCellNo * hit->GetXid() + fCellNo * hit->GetYid() + hit->GetZid();
            edmHitCore.energy = hit->GetEdep() * sim::g42edm::energy;
            ;
            auto position = fcc::Point();
            position.x = hit->GetPos().x() * sim::g42edm::length;
            position.y = hit->GetPos().y() * sim::g42edm::length;
            position.z = hit->GetPos().z() * sim::g42edm::length;
            debug() << "position of hit: " << hit->GetPos() << "mm " << endmsg;
            auto posHit = edmPositioned->create(position, edmHitCore);
            energyTotal += edmHitCore.energy;
          }
        }
        debug() << "\t" << hitNo << " hits are non-zero in collection #" << iter_coll << ": " << collect->GetName()
                << endmsg;
        debug() << "\t" << edmPositioned->size() << " hits are stored in EDM" << endmsg;
        debug() << "\t" << energyTotal << " GeV = total energy stored" << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}
