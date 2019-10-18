#include "SimG4SmearGenParticles.h"

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Common/Units.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/MCParticleCollection.h"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_COMPONENT(SimG4SmearGenParticles)

SimG4SmearGenParticles::SimG4SmearGenParticles(const std::string& aName,ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("inParticles", m_inParticles, "Handle for the input particles");
  declareProperty("smearedParticles", m_particles, "Handle for the particles to be written");
  declareProperty("smearTool", m_smearTool, "Handle to smear generated particles tool");
}

StatusCode SimG4SmearGenParticles::initialize() { 

  StatusCode sc = GaudiAlgorithm::initialize();
  // Use smearing tool                                                                                                                                                                                     
  if (!m_smearTool.retrieve()) {
    info() << "Generated particels will not be smeared!!!" << endmsg;
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4SmearGenParticles::execute() {

  auto particles = m_particles.createAndPut();
  const fcc::MCParticleCollection* coll = m_inParticles.get();
  info() << "Input particle collection size: " << coll->size() << endmsg;
  
  int n_part = 0;
  for (const auto& j : *coll) {
    const fcc::MCParticle& MCparticle = j;
    // save only charged particles, visible in tracker
    verbose() << "Charge of input particles: " << MCparticle.charge() << endmsg;

    if ( MCparticle.charge()!=0 || MCparticle.pdgId()==-211 || !m_simTracker){
      
      fcc::MCParticle particle = MCparticle.clone();
      // smear momentum according to trackers resolution
      CLHEP::Hep3Vector mom = CLHEP::Hep3Vector(MCparticle.p4().px, MCparticle.p4().py, MCparticle.p4().pz);
      //    m_smearTool->checkConditions(5000,10000000,6);
      m_smearTool->smearMomentum(mom, MCparticle.pdgId());
      particle.p4().px = mom.x();
      particle.p4().py = mom.y();
      particle.p4().pz = mom.z();
      
      n_part++;
      particles->push_back(particle);
    }
  }
  
  debug() << "\t" << n_part << " particles are stored in smeared particles collection" << endmsg;
  debug() << "Output particle collection size: " << particles->size() << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode SimG4SmearGenParticles::finalize() { return GaudiAlgorithm::finalize(); }
