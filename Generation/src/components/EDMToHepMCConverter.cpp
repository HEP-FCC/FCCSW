#include "EDMToHepMCConverter.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"


using HepMC::GenParticle;

DECLARE_COMPONENT(EDMToHepMCConverter)

EDMToHepMCConverter::EDMToHepMCConverter(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hepmc", m_hepmchandle, "HepMC event handle (output)");
  declareProperty("genparticles", m_genphandle, "Generated particles collection (input)");
  declareProperty("genvertices", m_genvhandle, "Generated vertices collection (input)");
}

StatusCode EDMToHepMCConverter::initialize() { return GaudiAlgorithm::initialize(); }

StatusCode EDMToHepMCConverter::execute() {

  const fcc::MCParticleCollection* particles = m_genphandle.get();
  // ownership of event given to data service at the end of execute
  HepMC::GenEvent* event = new HepMC::GenEvent;
  event->use_units(HepMC::Units::GEV, HepMC::Units::MM);


  for (auto p : *(particles)) {
    if (p.status() == 1) {  // only final state particles
      GenParticle* pHepMC =
          new GenParticle(HepMC::FourVector(p.p4().px, p.p4().py, p.p4().pz, p.p4().mass),
                          p.pdgId(),
                          p.status());  // hepmc status code for final state particle

      fcc::ConstGenVertex vStart = p.startVertex();
      if (p.startVertex().isAvailable()) {
        HepMC::GenVertex* v =
            new HepMC::GenVertex(HepMC::FourVector(vStart.position().x,
                                                   vStart.position().y,
                                                   vStart.position().z,
                                                   vStart.ctau() / Gaudi::Units::c_light));

        v->add_particle_out(pHepMC);
        event->add_vertex(v);
      }
    }
  }

  m_hepmchandle.put(event);
  return StatusCode::SUCCESS;
}

StatusCode EDMToHepMCConverter::finalize() { return GaudiAlgorithm::finalize(); }
