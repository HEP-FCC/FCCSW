#include "HepMCConverter.h"

#include "GaudiKernel/PhysicalConstants.h"

DECLARE_COMPONENT(HepMCConverter)

HepMCConverter::HepMCConverter(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc)
{
   declareInput("hepmc", m_hepmchandle);
   declareOutput("genparticles", m_genphandle);
   declareOutput("genvertices", m_genvhandle);
}

StatusCode HepMCConverter::initialize() {
   return GaudiAlgorithm::initialize();
}

StatusCode HepMCConverter::execute() {
   const HepMC::GenEvent* event = m_hepmchandle.get();
   MCParticleCollection* particles = new MCParticleCollection();
   GenVertexCollection* vertices = new GenVertexCollection();

   // conversion of units to mm and GeV
   double length_unit = HepMC::Units::conversion_factor(event->length_unit(), HepMC::Units::MM)*Gaudi::Units::mm;
   double mom_unit = HepMC::Units::conversion_factor(event->momentum_unit(),HepMC::Units::GEV)*Gaudi::Units::GeV;

   // currently only final state particles converted (no EndVertex as they didn't decay)
   // TODO add translation of decayed particles
   HepMC::FourVector tmp;
   for(auto vertex_i = event->vertices_begin();
       vertex_i != event->vertices_end(); ++vertex_i )
   {
      tmp = (*vertex_i)->position();
      GenVertexHandle vertex = vertices->create();
      Point& position = vertex.mod().Position;
      position.X = tmp.x()*length_unit;
      position.Y = tmp.y()*length_unit;
      position.Z = tmp.z()*length_unit;
      vertex.mod().Ctau = tmp.t()/Gaudi::Units::c_light/length_unit;

      for (auto particle_i = (*vertex_i)->particles_begin(HepMC::children);
           particle_i != (*vertex_i)->particles_end(HepMC::children);
           ++particle_i)
      {
         // take only final state particles
         if( (*particle_i)->status() != 1 ) continue;

         tmp = (*particle_i)->momentum();
         MCParticleHandle particle = particles->create();
         BareParticle& core = particle.mod().Core;
         core.Type = (*particle_i)->pdg_id();
         core.Status = (*particle_i)->status();
         core.P4.Px = tmp.px()*mom_unit;
         core.P4.Py = tmp.py()*mom_unit;
         core.P4.Pz = tmp.pz()*mom_unit;
         core.P4.Mass = (*particle_i)->generated_mass()*mom_unit;
         particle.mod().StartVertex = vertex;
      }
   }

   m_genphandle.put(particles);
   m_genvhandle.put(vertices);
   return StatusCode::SUCCESS;
}

StatusCode HepMCConverter::finalize() {
   return GaudiAlgorithm::finalize();
}
