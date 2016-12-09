#include "GenParticleFilter.h"

#include "datamodel/LorentzVector.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertex.h"

DECLARE_COMPONENT(GenParticleFilter)

GenParticleFilter::GenParticleFilter(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc),
  m_iGenpHandle("AllGenParticles", Gaudi::DataHandle::Reader, this),
  m_oGenpHandle("FilteredGenParticles", Gaudi::DataHandle::Writer, this)

{
  declareProperty("allGenParticles", m_iGenpHandle);
  declareProperty("filteredGenParticles", m_oGenpHandle);
}

StatusCode GenParticleFilter::initialize() {
  return GaudiAlgorithm::initialize();
}

StatusCode GenParticleFilter::execute() {
  const fcc::MCParticleCollection* inparticles = m_iGenpHandle.get();
  auto particles = m_oGenpHandle.createAndPut();
  bool accept = false;
  for (auto ptc : (*inparticles)) {
    accept = false;
    for (auto status : m_accept) {
      if (ptc.status() == status) {
        accept = true;
      }
      if (accept) {
        fcc::MCParticle outptc = particles->create(ptc.core());
        outptc.startVertex(ptc.startVertex());
        outptc.endVertex(ptc.endVertex());
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode GenParticleFilter::finalize() {
  return GaudiAlgorithm::finalize();
}
