#include "EDMToHepMCConverter.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "edm4hep/MCParticleCollection.h"


using HepMC::GenParticle;

DECLARE_COMPONENT(EDMToHepMCConverter)

EDMToHepMCConverter::EDMToHepMCConverter(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hepmc", m_hepmchandle, "HepMC event handle (output)");
  declareProperty("GenParticles", m_genphandle, "Generated particles collection (input)");
}

StatusCode EDMToHepMCConverter::initialize() { return GaudiAlgorithm::initialize(); }

StatusCode EDMToHepMCConverter::execute() {

  const edm4hep::MCParticleCollection* particles = m_genphandle.get();
  // ownership of event given to data service at the end of execute
  HepMC::GenEvent* event = new HepMC::GenEvent;
  event->use_units(HepMC::Units::GEV, HepMC::Units::MM);


  for (auto p : *(particles)) {
    if (p.getGeneratorStatus() == 1) {  // only final state particles
      edm4hep::Vector3f mom = p.getMomentum();
      GenParticle* pHepMC =
          new GenParticle(HepMC::FourVector(mom.x, mom.y, mom.z, p.getMass()),
                          p.getPDG(),
                          p.getGeneratorStatus());  // hepmc status code for final state particle

        edm4hep::Vector3d pos = p.getVertex();
        HepMC::GenVertex* v =
            new HepMC::GenVertex(HepMC::FourVector(pos.x,
                                                   pos.y,
                                                   pos.z,
                                                   p.getTime() / Gaudi::Units::c_light));

        v->add_particle_out(pHepMC);
        event->add_vertex(v);
    }
  }

  m_hepmchandle.put(event);
  return StatusCode::SUCCESS;
}

StatusCode EDMToHepMCConverter::finalize() { return GaudiAlgorithm::finalize(); }
