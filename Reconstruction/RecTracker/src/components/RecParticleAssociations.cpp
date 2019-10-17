
#include "DetInterface/IGeoSvc.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"

#include "DD4hep/Detector.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include <cmath>
#include <random>

#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/ITrackFittingTool.h"
#include "RecParticleAssociations.h"
#include "RecTracker/TrackingUtils.h"



DECLARE_COMPONENT(RecParticleAssociations)

RecParticleAssociations::RecParticleAssociations(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("TrackRecoParticles", m_recParticles, "TrackRecoParticles (Input)");
  declareProperty("SimParticles", m_simParticles, "SimParticles (Input)");
  declareProperty("RecSimParticleAssociations", m_associations, "ParticleAssociations (Output)");
}

StatusCode RecParticleAssociations::initialize() {
  debug() << "initialize" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode RecParticleAssociations::execute() {

  // get hits from event store
  const fcc::ParticleCollection* recParticles = m_recParticles.get();
  const fcc::MCParticleCollection* simParticles = m_simParticles.get();
  auto assoc = m_associations.createAndPut();
  for (auto pr: (*recParticles)) {
    for (auto ps: (*simParticles)) {
      if (pr.bits() == ps.bits()) {
        auto relation = assoc->create();
        relation.rec(pr);
        relation.sim(ps);
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode RecParticleAssociations::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
