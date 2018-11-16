
#include <cmath>

#include "GaudiKernel/SystemOfUnits.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

#include "ExtrapolationTest.h"

DECLARE_ALGORITHM_FACTORY(ExtrapolationTest)

ExtrapolationTest::ExtrapolationTest(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc), m_extrapolationTool(nullptr) {
  declareProperty("extrapolationTool", m_extrapolationTool,
                  "Pointer to extrapolation tool, needed to extrapolate through the tracker.");
  declareProperty("ExtrapolatedTrackStates", m_extrapolatedTrackStates, "ExtrapolatedTrackStates");
  declareProperty("genParticles", m_genParticles, "Handle for the EDM MC particles to be read");
}

StatusCode ExtrapolationTest::initialize() {

  // StatusCode sc = GaudiAlgorithm::initialize();
  // if (sc.isFailure()) return sc;
  // retrieve the extrapolation tool
  if (!m_extrapolationTool.retrieve()) {
    error() << "Extrapolation tool cannot be retrieved" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode ExtrapolationTest::execute() {

  // get the input mc particles
  const fcc::MCParticleCollection* mcparticles = m_genParticles.get();
  // create the TrackStateCollection to be written out
  auto exTrackStateCollection = m_extrapolatedTrackStates.createAndPut();
  // go through all particles to be extrapolated for this event
  for (const auto& mcparticle : *mcparticles) {

    // convert particle to trackstate
    auto vertex = mcparticle.startVertex();
    auto p4 = mcparticle.core().p4;
    double phi = std::atan2(p4.py, p4.px);
    double p3Mag = std::sqrt(std::pow(p4.px, 2) + std::pow(p4.py, 2) + std::pow(p4.pz, 2));
    double theta = std::acos(p4.pz / p3Mag);
    double qOverP = 1. / p3Mag * mcparticle.charge() * -1;
    double d0 = 0;
    double z0 = 0;
    auto theTrackState = fcc::TrackState(phi, theta, qOverP, d0, z0, vertex.position(), std::array<float, 15ul>());

    debug() << "start extrapolation ..." << endmsg;
    auto stateVector = m_extrapolationTool->extrapolate(theTrackState);
    for (auto t : stateVector) {
      auto outTrackstate = exTrackStateCollection->create();
      outTrackstate.phi(t.phi());
      outTrackstate.theta(t.theta());
      outTrackstate.qOverP(t.qOverP());
      outTrackstate.d0(t.d0());
      outTrackstate.z0(t.z0());
      outTrackstate.referencePoint(t.referencePoint());
      // todo add covariance
    }
  }

  debug() << "Extrapolation finished after " << exTrackStateCollection->size() << " extrapolation steps" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode ExtrapolationTest::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
