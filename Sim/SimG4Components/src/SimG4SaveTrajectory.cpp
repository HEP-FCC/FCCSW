#include "SimG4SaveTrajectory.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "SimG4Common/Units.h"

// Geant4
#include "G4Event.hh"
#include "G4Trajectory.hh"
#include "G4VTrajectory.hh"


// datamodel
#include "edm4hep/TrackerHitCollection.h"

DECLARE_COMPONENT(SimG4SaveTrajectory)

SimG4SaveTrajectory::SimG4SaveTrajectory(const std::string& aType, const std::string& aName,
                                           const IInterface* aParent)
    : GaudiTool(aType, aName, aParent), m_geoSvc("GeoSvc", aName) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("TrajectoryPoints", m_positionedTrackHits, "Handle for trajectory hits");
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
  G4TrajectoryContainer* trajectoryContainer = aEvent.GetTrajectoryContainer();
  for (size_t trajectoryIndex = 0; trajectoryIndex < trajectoryContainer->size(); ++trajectoryIndex) {
    G4VTrajectory* theTrajectory =  (*trajectoryContainer)[trajectoryIndex];
    for (int pointIndex = 0; pointIndex < theTrajectory->GetPointEntries(); ++pointIndex) {
      auto trajectoryPoint = theTrajectory->GetPoint(pointIndex)->GetPosition();
      edm4hep::TrackerHit edmHit = edmPositions->create();
      edmHit.setCellID(0);
      edmHit.setEnergy(0);
      edmHit.setTime(0;
      edmHit.setPosition({
        trajectoryPoint.x() * sim::g42edm::length;
        trajectoryPoint.y() * sim::g42edm::length;
        trajectoryPoint.z() * sim::g42edm::length;
      });
    }
  }

  return StatusCode::SUCCESS;
}
