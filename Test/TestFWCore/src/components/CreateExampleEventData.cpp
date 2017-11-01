#include "CreateExampleEventData.h"


// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"


DECLARE_ALGORITHM_FACTORY(CreateExampleEventData)

CreateExampleEventData::CreateExampleEventData(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("genparticles", m_genParticleHandle, "Dummy Particle collection (output)");
  declareProperty("genvertices", m_genVertexHandle, "Dummy Vertex collection (output)");
  declareProperty("calohits", m_caloHitHandle, "Dummy Hit collection (output)");
  declareProperty("trackhits", m_trackHitHandle, "Dummy Hit collection (output)");
  declareProperty("positionedcalohits", m_posCaloHitHandle, "Dummy Positioned Hit collection (output)");
  declareProperty("positionedtrackhits", m_posTrackHitHandle, "Dummy Positioned Hit collection (output)");
}

CreateExampleEventData::~CreateExampleEventData() {}

StatusCode CreateExampleEventData::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CreateExampleEventData::execute() {

  fcc::MCParticleCollection* particles = m_genParticleHandle.createAndPut();
  fcc::GenVertexCollection* vertices = m_genVertexHandle.createAndPut();
  fcc::MCParticle particle = particles->create();
  auto vertex = vertices->create();
  auto& position = vertex.position();
  position.x = m_magicNumberOffset + 1;
  position.y = m_magicNumberOffset + 2;
  position.z = m_magicNumberOffset + 3;
  vertex.ctau(m_magicNumberOffset + 4);

  auto& p4 = particle.p4();
  p4.px = m_magicNumberOffset + 5; 
  p4.py = m_magicNumberOffset + 6;
  p4.pz = m_magicNumberOffset + 7;
  p4.mass = m_magicNumberOffset + 8;
  particle.startVertex(vertex);

  fcc::TrackHitCollection* trackhits = m_trackHitHandle.createAndPut();
  fcc::PositionedTrackHitCollection* positionedtrackhits = m_posTrackHitHandle.createAndPut();

  fcc::CaloHitCollection* calohits = m_caloHitHandle.createAndPut();
  fcc::PositionedCaloHitCollection* positionedcalohits =  m_posCaloHitHandle.createAndPut();

  auto trackHitPosition = fcc::Point();
  trackHitPosition.x = m_magicNumberOffset + 9;
  trackHitPosition.y = m_magicNumberOffset + 10; 
  trackHitPosition.z = m_magicNumberOffset + 11;
  auto edmTrackHit = trackhits->create();
  auto& edmTrackHitCore = edmTrackHit.core();
  edmTrackHitCore.cellId = m_magicNumberOffset + 12;
  edmTrackHitCore.energy = m_magicNumberOffset + 13;
  positionedtrackhits->create(trackHitPosition, edmTrackHitCore);

  auto caloHitPosition = fcc::Point();
  caloHitPosition.x = m_magicNumberOffset + 14;
  caloHitPosition.y = m_magicNumberOffset + 15; 
  caloHitPosition.z = m_magicNumberOffset + 16;
  auto edmCaloHit = calohits->create();
  auto& edmCaloHitCore = edmCaloHit.core();
  edmCaloHitCore.cellId = m_magicNumberOffset + 17;
  edmCaloHitCore.energy = m_magicNumberOffset + 18;
  positionedcalohits->create(caloHitPosition, edmCaloHitCore);


  return StatusCode::SUCCESS;
}

StatusCode CreateExampleEventData::finalize() { return GaudiAlgorithm::finalize(); }
