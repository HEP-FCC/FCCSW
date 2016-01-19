#include "G4SaveSmearedParticles.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/ParticleInformation.h"

// Geant4
#include "G4Event.hh"

// albers
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCAssociationCollection.h"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_TOOL_FACTORY(G4SaveSmearedParticles)

G4SaveSmearedParticles::G4SaveSmearedParticles(const std::string& aType, const std::string& aName, const IInterface* aParent) :
GaudiTool(aType, aName, aParent) {
  declareInterface<IG4SaveOutputTool>(this);
  declareOutput("particles", m_particles,"particles/smearedParticles");
  declareOutput("particlesMCparticles", m_particlesMCparticles,"particles/smearedParticles");
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

G4SaveSmearedParticles::~G4SaveSmearedParticles() {}

StatusCode G4SaveSmearedParticles::initialize() {
  return GaudiTool::initialize();
}

StatusCode G4SaveSmearedParticles::finalize() {
  return GaudiTool::finalize();
}

StatusCode G4SaveSmearedParticles::saveOutput(const G4Event& aEvent) {
  ParticleCollection* particles = new ParticleCollection();
  ParticleMCAssociationCollection* associations = new ParticleMCAssociationCollection();
  for(int i=0;i<aEvent.GetNumberOfPrimaryVertex();i++) {
    for(int j=0;j<aEvent.GetPrimaryVertex(i)->GetNumberOfParticle();j++) {
      const G4PrimaryParticle* g4particle = aEvent.GetPrimaryVertex(i)->GetPrimary(j);
      sim::ParticleInformation* info = dynamic_cast<sim::ParticleInformation*>(g4particle->GetUserInformation());
      const MCParticleHandle& MCparticle = info->mcParticleHandle();
      ParticleHandle particle = particles->create();
      ParticleMCAssociationHandle association = associations->create();
      association.mod().Rec = particle;
      association.mod().Sim = MCparticle;
      BareParticle& core = particle.mod().Core;
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
    }
  }
  m_particles.put(particles);
  m_particlesMCparticles.put(associations);
  return StatusCode::SUCCESS;
}
