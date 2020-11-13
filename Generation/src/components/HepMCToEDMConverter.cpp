#include "HepMCToEDMConverter.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "edm4hep/MCParticleCollection.h"
#include "HepPDT/ParticleID.hh"

DECLARE_COMPONENT(HepMCToEDMConverter)


edm4hep::MCParticle convert(HepMC::GenParticle* hepmcParticle) {
    edm4hep::MCParticle edm_particle;
    edm_particle.setPDG(hepmcParticle->pdg_id());
    edm_particle.setGeneratorStatus(hepmcParticle->status());
    // look up charge from pdg_id
    HepPDT::ParticleID particleID(hepmcParticle->pdg_id());
    edm_particle.setCharge(particleID.charge());
    //  convert momentum
    auto p = hepmcParticle->momentum();
    edm_particle.setMomentum( {float(p.px()), float(p.py()), float(p.pz())} );
    return edm_particle;
 }

HepMCToEDMConverter::HepMCToEDMConverter(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hepmc", m_hepmchandle, "HepMC event handle (input)");
  declareProperty("GenParticles", m_genphandle, "Generated particles collection (output)");
}

StatusCode HepMCToEDMConverter::initialize() { 
  return GaudiAlgorithm::initialize();
}

StatusCode HepMCToEDMConverter::execute() {
  const HepMC::GenEvent* evt = m_hepmchandle.get();
  edm4hep::MCParticleCollection* particles = new edm4hep::MCParticleCollection();

  
  std::unordered_map<unsigned int, edm4hep::MCParticle> _map;
  for (auto _p = evt->particles_begin(); _p != evt->particles_end(); ++_p) {
    debug() << "Converting hepmc particle with Pdg_ID \t" << (*_p)->pdg_id() << "and barcode \t" <<  (*_p)->barcode() << endmsg;
    if (_map.find((*_p)->barcode()) == _map.end()) {
      edm4hep::MCParticle edm_particle = convert(*_p);
      _map.insert({(*_p)->barcode(), edm_particle});
    }
    // mother/daughter links
    auto prodvertex = (*_p)->production_vertex();
    if (nullptr != prodvertex) {
      for (auto particle_mother = prodvertex->particles_in_const_begin();
                particle_mother != prodvertex->particles_in_const_end(); 
                ++particle_mother) {
        if (_map.find((*particle_mother)->barcode()) == _map.end()) {
          edm4hep::MCParticle edm_particle = convert(*particle_mother);
          _map.insert({(*particle_mother)->barcode(), edm_particle});
        }
        _map[(*_p)->barcode()].addToParents(_map[(*particle_mother)->barcode()]);
      }
    }
    auto endvertex = (*_p)->end_vertex();
    if (nullptr != endvertex) {
      for (auto particle_daughter = endvertex->particles_out_const_begin();
           particle_daughter != endvertex->particles_out_const_end();
           ++particle_daughter) {
        if (_map.find((*particle_daughter)->barcode()) == _map.end()) {
          auto edm_particle = convert(*particle_daughter);
          _map.insert({(*particle_daughter)->barcode(), edm_particle});
        }
        _map[(*_p)->barcode()].addToDaughters(_map[(*particle_daughter)->barcode()]);
      }
    }
  }
  for (auto particle_pair: _map) {
    particles->push_back(particle_pair.second);
  }
  m_genphandle.put(particles);
  return StatusCode::SUCCESS;
}

StatusCode HepMCToEDMConverter::finalize() {
   return GaudiAlgorithm::finalize();
}
