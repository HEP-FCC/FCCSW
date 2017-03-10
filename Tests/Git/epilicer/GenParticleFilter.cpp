#include "GenParticleFilter.h"
#include "DataObjects/LorentzVector.h"

DECLARE_COMPONENT(GenParticleFilter)

GenParticleFilter::GenParticleFilter(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc)
{
  declareInput("genparticles", m_genphandle);
  declareOutput("filterparticles", m_filterpart);
}

StatusCode GenParticleFilter::initialize() {
  return GaudiAlgorithm::initialize();
}

StatusCode GenParticleFilter::execute() {

  const ParticleCollection* particles = m_genphandle.get();
  ParticleCollection* fparticles = new ParticleCollection();

  for (auto it = particles->begin(); it != particles->end(); ++it) {
    auto p = *it;

	if (p.Status() == 1) {
		ParticleHandle& outptc = fparticles->create();
		outptc.setID(p.ID());
		outptc.setStatus(p.Status());
		/*std::cout << p.ID() << "\t" << p.Status() << "\t" 
			      << p.P4().Pt << "\t" << p.P4().Eta << "\t"
		          << p.P4().Phi << "\t" << p.P4().Mass << "\t"
				  << std::endl;*/
		LorentzVector p4;
		p4.Pt = p.P4().Pt;
		p4.Eta = p.P4().Eta;
		p4.Phi = p.P4().Phi;
		p4.Mass = p.P4().Mass;
		outptc.setP4(p4);
	}
  }
  m_filterpart.put(fparticles);
  return StatusCode::SUCCESS;
}

StatusCode GenParticleFilter::finalize() {
  return GaudiAlgorithm::finalize();
}
