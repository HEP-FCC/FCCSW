#include "HepMCToEDMConverter.h"

#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ParticleProperty.h"

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

StatusCode HepMCToEDMConverter::initialize() { 
  return GaudiAlgorithm::initialize();
   }

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
  
  // bookkeeping of particle / vertex relations
  std::unordered_map<const HepMC::GenVertex*, fcc::GenVertex> hepmcToEdmVertexMap;
  HepMC::FourVector tmp; /// temp variable for the transfer of position / momentom
  // iterate over particles in event
  for (auto particle_i = event->particles_begin(); particle_i != event->particles_end(); ++particle_i) {

    // if there is a list of statuses to filter: filter by status
    if(std::find(m_hepmcStatusList.begin(), m_hepmcStatusList.end(), (*particle_i)->status()) == m_hepmcStatusList.end() && m_hepmcStatusList.size() > 0) continue;
    // create edm 
    fcc::MCParticle particle = particles->create();
    // set mcparticle data members
    particle.pdgId((*particle_i)->pdg_id());
    particle.status((*particle_i)->status());
    /// lookup charge in particle properties
    HepPDT::ParticleID particleID((*particle_i)->pdg_id());
    particle.charge(particleID.charge());

    auto& p4 = particle.p4();
    tmp = (*particle_i)->momentum();
    p4.px = tmp.px() * hepmc2EdmEnergy;
    p4.py = tmp.py() * hepmc2EdmEnergy;
    p4.pz = tmp.pz() * hepmc2EdmEnergy;
    p4.mass = (*particle_i)->generated_mass() * hepmc2EdmEnergy;

    /// create production vertex, if it has not already been created and logged in the map
    HepMC::GenVertex* productionVertex = (*particle_i)->production_vertex();
    if (nullptr != productionVertex) {
      if (hepmcToEdmVertexMap.find(productionVertex) != hepmcToEdmVertexMap.end()) {
        // vertex already in map, no need to create a new one
        particle.startVertex(hepmcToEdmVertexMap[productionVertex]);
      } else {
        tmp = productionVertex->position();
        auto vertex = vertices->create();
        auto& position = vertex.position();
        position.x = tmp.x() * hepmc2EdmLength;
        position.y = tmp.y() * hepmc2EdmLength;
        position.z = tmp.z() * hepmc2EdmLength;
        vertex.ctau(tmp.t() * Gaudi::Units::c_light * hepmc2EdmLength);  // is ctau like this?
        // add vertex to map for further particles
        hepmcToEdmVertexMap.insert({productionVertex, vertex});
        particle.startVertex(vertex);
      }
    }

    /// create decay vertex, if it has not already been created and logged in the map
    HepMC::GenVertex* decayVertex = (*particle_i)->end_vertex();
    if (nullptr != decayVertex) {
      if (hepmcToEdmVertexMap.find(decayVertex) != hepmcToEdmVertexMap.end()) {
        // vertex already in map, no need to create a new one
        particle.endVertex(hepmcToEdmVertexMap[decayVertex]);
      } else {
        tmp = decayVertex->position();
        auto vertex = vertices->create();
        auto& position = vertex.position();
        position.x = tmp.x() * hepmc2EdmLength;
        position.y = tmp.y() * hepmc2EdmLength;
        position.z = tmp.z() * hepmc2EdmLength;
        vertex.ctau(tmp.t() * Gaudi::Units::c_light * hepmc2EdmLength);  // is ctau like this?
        // add vertex to map for further particles
        hepmcToEdmVertexMap.insert({decayVertex, vertex});
        particle.endVertex(vertex);
      }
    }

  } // particle loop

  m_genphandle.put(particles);
  m_genvhandle.put(vertices);
  return StatusCode::SUCCESS;
}

StatusCode HepMCToEDMConverter::finalize() {
   return GaudiAlgorithm::finalize(); }
