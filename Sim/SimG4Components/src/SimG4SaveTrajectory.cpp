#include "SimG4SaveTrajectory.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "SimG4Common/Units.h"

// Geant4
#include "G4Event.hh"
#include "G4Trajectory.hh"
#include "G4VTrajectory.hh"


// datamodel
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_TOOL_FACTORY(SimG4SaveTrajectory)

SimG4SaveTrajectory::SimG4SaveTrajectory(const std::string& aType, const std::string& aName,
                                           const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("trajectoryPoints", m_positionedTrackHits, "Handle for tracker hits");
  declareProperty("trajectory", m_trackHits, "Handle for tracker hits including position information");
}

SimG4SaveTrajectory::~SimG4SaveTrajectory() {}

StatusCode SimG4SaveTrajectory::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4SaveTrajectory::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4SaveTrajectory::saveOutput(const G4Event& aEvent) {
  auto edmPositions = m_positionedTrackHits.createAndPut();
  auto edmHits = m_trackHits.createAndPut();

  G4TrajectoryContainer* trajectoryContainer = aEvent.GetTrajectoryContainer();
  for (size_t trajectoryIndex = 0; trajectoryIndex < trajectoryContainer->size(); ++trajectoryIndex) {
    G4VTrajectory* theTrajectory =  (*trajectoryContainer)[trajectoryIndex];
    for (int pointIndex = 0; pointIndex < theTrajectory->GetPointEntries(); ++pointIndex) {
      auto trajectoryPoint = theTrajectory->GetPoint(pointIndex)->GetPosition();
      fcc::TrackHit edmHit = edmHits->create();
      fcc::BareHit& edmHitCore = edmHit.core();
      edmHitCore.cellId = 0;
      edmHitCore.energy = 0;
      edmHitCore.time = 0;
      auto position = fcc::Point();
      position.x = trajectoryPoint.x() * sim::g42edm::length;
      position.y = trajectoryPoint.y() * sim::g42edm::length;
      position.z = trajectoryPoint.z() * sim::g42edm::length;
      edmPositions->create(position, edmHitCore);
    }
  }

  return StatusCode::SUCCESS;
}
