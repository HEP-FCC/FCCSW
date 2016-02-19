#include "HepMCConverter.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"

#include "Generation/Units.h"

DECLARE_COMPONENT(HepMCConverter)

HepMCConverter::HepMCConverter(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) {
  declareInput("hepmc", m_hepmchandle);
  declareOutput("genparticles", m_genphandle);
  declareOutput("genvertices", m_genvhandle);
  }

StatusCode HepMCConverter::initialize() {
  return GaudiAlgorithm::initialize();
}

StatusCode HepMCConverter::execute() {
  const HepMC::GenEvent* event = m_hepmchandle.get();
  fcc::MCParticleCollection* particles = new fcc::MCParticleCollection();
  fcc::GenVertexCollection* vertices = new fcc::GenVertexCollection();

  // conversion of units to EDM standard units:
  // First cover the case that hepMC file is not in expected units and then convert to EDM default
  double hepmc2EdmLength = HepMC::Units::conversion_factor(event->length_unit(), gen::hepmcdefault::length) * gen::hepmc2edm::length;
  double hepmc2EdmEnergy = HepMC::Units::conversion_factor(event->momentum_unit(), gen::hepmcdefault::energy) * gen::hepmc2edm::energy;

  // currently only final state particles converted (no EndVertex as they didn't decay)
  // TODO add translation of decayed particles
  HepMC::FourVector tmp;
  for(auto vertex_i = event->vertices_begin();
      vertex_i != event->vertices_end(); ++vertex_i ) {
    tmp = (*vertex_i)->position();
    auto vertex = vertices->create();
    auto& position = vertex.Position();
    position.X = tmp.x()*hepmc2EdmLength;
    position.Y = tmp.y()*hepmc2EdmLength;
    position.Z = tmp.z()*hepmc2EdmLength;
    vertex.Ctau(tmp.t()*Gaudi::Units::c_light*hepmc2EdmLength); // is ctau like this?

    for (auto particle_i = (*vertex_i)->particles_begin(HepMC::children);
         particle_i != (*vertex_i)->particles_end(HepMC::children);
         ++particle_i) {
      // take only final state particles
      if( (*particle_i)->status() != 1 ) continue;

      tmp = (*particle_i)->momentum();
      fcc::MCParticle particle = particles->create();
      fcc::BareParticle& core = particle.Core();
      core.Type = (*particle_i)->pdg_id();
      core.Status = (*particle_i)->status();
      core.P4.Px = tmp.px()*hepmc2EdmEnergy;
      core.P4.Py = tmp.py()*hepmc2EdmEnergy;
      core.P4.Pz = tmp.pz()*hepmc2EdmEnergy;
      core.P4.Mass = (*particle_i)->generated_mass()*hepmc2EdmEnergy;
      particle.StartVertex(vertex);
    }
  }

  m_genphandle.put(particles);
  m_genvhandle.put(vertices);
  return StatusCode::SUCCESS;
}

StatusCode HepMCConverter::finalize() {
  return GaudiAlgorithm::finalize();
}

