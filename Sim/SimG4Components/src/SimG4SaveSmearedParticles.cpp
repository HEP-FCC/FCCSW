#include "SimG4SaveSmearedParticles.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/ParticleInformation.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_TOOL_FACTORY(SimG4SaveSmearedParticles)

SimG4SaveSmearedParticles::SimG4SaveSmearedParticles(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareOutput("particles", m_particles,"particles/smearedParticles");
  declareOutput("particlesMCparticles", m_particlesMCparticles,"particles/smearedParticles");
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

SimG4SaveSmearedParticles::~SimG4SaveSmearedParticles() {}

StatusCode SimG4SaveSmearedParticles::initialize() {
  return GaudiTool::initialize();
}

StatusCode SimG4SaveSmearedParticles::finalize() {
  return GaudiTool::finalize();
}

StatusCode SimG4SaveSmearedParticles::saveOutput(const G4Event& aEvent) {
  auto particles = m_particles.createAndPut();
  auto associations = m_particlesMCparticles.createAndPut();
  int n_part=0;
  for(int i=0;i<aEvent.GetNumberOfPrimaryVertex();i++) {
    for(int j=0;j<aEvent.GetPrimaryVertex(i)->GetNumberOfParticle();j++) {
      const G4PrimaryParticle* g4particle = aEvent.GetPrimaryVertex(i)->GetPrimary(j);
      sim::ParticleInformation* info = dynamic_cast<sim::ParticleInformation*>(g4particle->GetUserInformation());
      if(info->smeared()) {
        const fcc::MCParticle& MCparticle = info->mcParticle();
        fcc::Particle particle = particles->create();
        fcc::ParticleMCParticleAssociation association = associations->create();
        association.Rec(particle);
        association.Sim(MCparticle);
        fcc::BareParticle& core = particle.Core();
        core.Type = g4particle->GetPDGcode();
        core.Status = 1; // how it is defined ???? as in HepMC ?
        core.Charge = g4particle->GetCharge();
        core.P4.Px = info->endMomentum().x()*sim::g42edm::energy;
        core.P4.Py = info->endMomentum().y()*sim::g42edm::energy;
        core.P4.Pz = info->endMomentum().z()*sim::g42edm::energy;
        core.P4.Mass = g4particle->GetMass()*sim::g42edm::energy;
        core.Vertex.X = info->vertexPosition().x()*sim::g42edm::length;
        core.Vertex.Y = info->vertexPosition().y()*sim::g42edm::length;
        core.Vertex.Z = info->vertexPosition().z()*sim::g42edm::length;
        n_part++;
      }
    }
  }
  debug() << "\t" << n_part<< " particles are stored in smeared particles collection"<<endmsg;
  return StatusCode::SUCCESS;
}
