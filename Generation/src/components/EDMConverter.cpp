#include "EDMConverter.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

#include "Generation/Units.h"

DECLARE_COMPONENT(EDMConverter)

EDMConverter::EDMConverter(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hepmc", m_hepmchandle, "HepMC event handle (output)");
  declareProperty("genparticles", m_genphandle, "Generated particles collection (input)");
  declareProperty("genvertices", m_genvhandle, "Generated vertices collection (input)");
}

StatusCode EDMConverter::initialize() { return GaudiAlgorithm::initialize(); }

StatusCode EDMConverter::execute() {

  const fcc::MCParticleCollection* particles = m_genphandle.get();
  // ownership of event given to data service at the end of execute
  HepMC::GenEvent* event = new HepMC::GenEvent;

  // conversion of units to EDM standard units:
  // First cover the case that hepMC file is not in expected units and then convert to EDM default
  double hepmc2EdmLength =
      HepMC::Units::conversion_factor(event->length_unit(), gen::hepmcdefault::length) * gen::hepmc2edm::length;
  double hepmc2EdmEnergy =
      HepMC::Units::conversion_factor(event->momentum_unit(), gen::hepmcdefault::energy) * gen::hepmc2edm::energy;

  for (auto p : *(particles)) {
    if (p.status() == 1) {  // only final state particles
      HepMC::GenParticle* pHepMC =
          new HepMC::GenParticle(HepMC::FourVector(p.p4().px, p.p4().py, p.p4().pz, p.p4().mass / hepmc2EdmEnergy),
                                 p.pdgId(),
                                 p.status());  // hepmc status code for final state particle

      fcc::ConstGenVertex vStart = p.startVertex();
      if (p.startVertex().isAvailable()) {
        HepMC::GenVertex* v =
            new HepMC::GenVertex(HepMC::FourVector(vStart.position().x / hepmc2EdmLength,
                                                   vStart.position().y / hepmc2EdmLength,
                                                   vStart.position().z / hepmc2EdmLength,
                                                   vStart.ctau() / Gaudi::Units::c_light / hepmc2EdmLength));

        v->add_particle_out(pHepMC);
        event->add_vertex(v);
      }
    }
  }

  m_hepmchandle.put(event);
  return StatusCode::SUCCESS;
}

StatusCode EDMConverter::finalize() { return GaudiAlgorithm::finalize(); }
