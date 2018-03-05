#include "DetInterface/IGeoSvc.h"
#include "DetInterface/ITrackingGeoSvc.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "SimG4Common/Units.h"

#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "Math/Math.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

#include <cmath>
#include <random>

#include "ExtrapolationTest.h"

using namespace Acts;
using DefaultCovMatrix = ActsSymMatrix<ParValue_t, NGlobalPars>;

DECLARE_ALGORITHM_FACTORY(ExtrapolationTest)

ExtrapolationTest::ExtrapolationTest(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc), m_extrapolationTool(nullptr) {
  declareProperty("extrapolationTool", m_extrapolationTool,
                  "Pointer to extrapolation tool, needed to extrapolate through the tracker.");
  declareProperty("positionedTrackHits", m_positionedTrackHits, "hits/TrackerPositionedHits");
  declareProperty("genParticles", m_genParticles, "Handle for the EDM MC particles to be read");
}

StatusCode ExtrapolationTest::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  // retrieve the extrapolation tool
  if (!m_extrapolationTool.retrieve()) {
    error() << "Extrapolation tool cannot be retieved" << endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

StatusCode ExtrapolationTest::execute() {

  // get the input mc particles
  const fcc::MCParticleCollection* mcparticles = m_genParticles.get();
  // create the PositionedTrackHitCollection to be written out
  auto posHitCollection = m_positionedTrackHits.createAndPut();
  // go through all particles to be extrapolated for this event
  for (const auto& mcparticle : *mcparticles) {
    // translate input first
    // (1) vertex
    const fcc::ConstGenVertex& v = mcparticle.startVertex();
    fcc::Vector3D vertex(v.x() * sim::edm2acts::length, v.y() * sim::edm2acts::length, v.z() * sim::edm2acts::length);
    // (2) momentum
    const fcc::BareParticle& mccore = mcparticle.core();
    fcc::Vector3D momentum(mccore.p4.px * sim::edm2acts::energy,
                           mccore.p4.py * sim::edm2acts::energy,
                           mccore.p4.pz * sim::edm2acts::energy);
    // (3) charge
    auto charge = mccore.charge;
    // make extrapolation using extrapolation tool
    debug() << "start extrapolation ..." << endmsg;
    Acts::ExtrapolationCell<Acts::TrackParameters> exCell = m_extrapolationTool->extrapolate(vertex, momentum, charge);
    debug() << "got " << exCell.extrapolationSteps.size() << " extrapolation steps" << endmsg;

    // write out extrapolation steps
    for (const auto& step : exCell.extrapolationSteps) {
      const auto& tp = step.parameters;
      if (tp) {
        fcc::TrackHit edmHit;
        fcc::BareHit& edmHitCore = edmHit.core();
        auto position = fcc::Point();
        position.x = tp->position().x();
        position.y = tp->position().y();
        position.z = tp->position().z();
        posHitCollection->create(position, edmHitCore);
      }  // if track parameters
    }    // go through steps

  }  // go through particles
  return StatusCode::SUCCESS;
}

StatusCode ExtrapolationTest::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
