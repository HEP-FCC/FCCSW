#include "CreateExampleEventData.h"


// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/FloatValueData.h"


DECLARE_COMPONENT(CreateExampleEventData)

CreateExampleEventData::CreateExampleEventData(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {
}

CreateExampleEventData::~CreateExampleEventData() {}

StatusCode CreateExampleEventData::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CreateExampleEventData::execute() {

  std::vector<int>* ff3 = m_someinthandle.createAndPut();
  ff3->emplace_back(777);

  std::vector<float>* ff2 = m_somefloatHandle2.createAndPut();

  ff2->emplace_back( 125.);
  ff2->emplace_back( 25.);

  Int_t* ff4 = new Int_t(123455);
  m_intHandle.put(ff4);

  float* ff5 = new float(0.54321);
  m_floatHandle.put(ff5);

  double* ff6 = new double(0.54321);
  m_doubleHandle.put(ff6);

  std::vector<fcc::FloatValueData>* ff = m_somefloatHandle.createAndPut();
  auto fff = fcc::FloatValueData();
  fff.value = 12345.;
  ff->emplace_back( fff);
  fff.value = 1/12.;
  ff->emplace_back( fff);

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
