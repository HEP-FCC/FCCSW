#include "HepMCToEDMConverter.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

#include "Generation/Units.h"
#include "HepPDT/ParticleID.hh"

DECLARE_COMPONENT(HepMCToEDMConverter)

HepMCToEDMConverter::HepMCToEDMConverter(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hepmc", m_hepmchandle, "HepMC event handle (input)");
  declareProperty("genparticles", m_genphandle, "Generated particles collection (output)");
  declareProperty("genvertices", m_genvhandle, "Generated vertices collection (output)");
}

StatusCode HepMCToEDMConverter::initialize() { return GaudiAlgorithm::initialize(); }

StatusCode HepMCToEDMConverter::execute() {
  const HepMC::GenEvent* event = m_hepmchandle.get();
  fcc::MCParticleCollection* particles = new fcc::MCParticleCollection();
  fcc::GenVertexCollection* vertices = new fcc::GenVertexCollection();

  // conversion of units to EDM standard units:
  // First cover the case that hepMC file is not in expected units and then convert to EDM default
  double hepmc2EdmLength =
      HepMC::Units::conversion_factor(event->length_unit(), gen::hepmcdefault::length) * gen::hepmc2edm::length;
  double hepmc2EdmEnergy =
      HepMC::Units::conversion_factor(event->momentum_unit(), gen::hepmcdefault::energy) * gen::hepmc2edm::energy;

  // currently only final state particles converted (no EndVertex as they didn't decay)
  // TODO add translation of decayed particles
  HepMC::FourVector tmp;
  for (auto vertex_i = event->vertices_begin(); vertex_i != event->vertices_end(); ++vertex_i) {
    tmp = (*vertex_i)->position();
    auto vertex = vertices->create();
    auto& position = vertex.position();
    position.x = tmp.x() * hepmc2EdmLength;
    position.y = tmp.y() * hepmc2EdmLength;
    position.z = tmp.z() * hepmc2EdmLength;
    vertex.ctau(tmp.t() * Gaudi::Units::c_light * hepmc2EdmLength);  // is ctau like this?

    for (auto particle_i = (*vertex_i)->particles_begin(HepMC::children);
         particle_i != (*vertex_i)->particles_end(HepMC::children);
         ++particle_i) {
      // take only final state particles
      if ((*particle_i)->status() != 1) continue;

      tmp = (*particle_i)->momentum();
      fcc::MCParticle particle = particles->create();
      particle.pdgId((*particle_i)->pdg_id());
      particle.status((*particle_i)->status());
      HepPDT::ParticleID particleID((*particle_i)->pdg_id());
      particle.charge(particleID.charge());
      auto& p4 = particle.p4();
      p4.px = tmp.px() * hepmc2EdmEnergy;
      p4.py = tmp.py() * hepmc2EdmEnergy;
      p4.pz = tmp.pz() * hepmc2EdmEnergy;
      p4.mass = (*particle_i)->generated_mass() * hepmc2EdmEnergy;
      particle.startVertex(vertex);
    }
  }

  m_genphandle.put(particles);
  m_genvhandle.put(vertices);
  return StatusCode::SUCCESS;
}

StatusCode HepMCToEDMConverter::finalize() { return GaudiAlgorithm::finalize(); }
