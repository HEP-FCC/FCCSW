#include "DelphesSaveGenParticles.h"

// Delphes
#include "modules/Delphes.h"
#include "classes/DelphesClasses.h"

// datamodel
#include "datamodel/MCParticleCollection.h"
// ROOT
#include "TObjArray.h"
#include "SimDelphesInterface/ParticleStatus.h"

DECLARE_TOOL_FACTORY(DelphesSaveGenParticles)

  DelphesSaveGenParticles::DelphesSaveGenParticles(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  GaudiTool(aType, aName, aParent) {
  declareInterface<IDelphesSaveOutputTool>(this);
  declareOutput("particles", m_particles);
  declareProperty("delphesArrayName", m_delphesArrayName);
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

DelphesSaveGenParticles::~DelphesSaveGenParticles() {}

StatusCode DelphesSaveGenParticles::initialize() {
  return GaudiTool::initialize();
}

StatusCode DelphesSaveGenParticles::finalize() {
  return GaudiTool::finalize();
}

StatusCode DelphesSaveGenParticles::saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) {
  // Create the collections
  auto colParticles = m_particles.createAndPut();

  const TObjArray* delphesColl = delphes.ImportArray(m_delphesArrayName.c_str());
  if (delphesColl == nullptr) {
    warning() << "Delphes collection " << m_delphesArrayName << " not present. Skipping it." << endmsg;
    return StatusCode::SUCCESS;
  }
  // ! FIXME: when Delphes skimmed gen collection will have proerly stored mother/daughter indices,
  //          add start/end vertex information here.

  // Save MC particles and vertices
  for(int j=0; j<delphesColl->GetEntries(); j++) {

    auto cand     = static_cast<Candidate *>(delphesColl->At(j));
    auto particle = colParticles->create();

    auto& barePart    = particle.core();
    barePart.pdgId    = cand->PID;
    barePart.status   = cand->Status;
    barePart.p4.px    = cand->Momentum.Px();
    barePart.p4.py    = cand->Momentum.Py();
    barePart.p4.pz    = cand->Momentum.Pz();
    barePart.p4.mass  = cand->Momentum.M();
    barePart.charge   = cand->Charge;
    barePart.vertex.x = cand->Position.X();
    barePart.vertex.y = cand->Position.Y();
    barePart.vertex.z = cand->Position.Z();

    if (cand->M1==-1)      barePart.bits = static_cast<unsigned>(ParticleStatus::kBeam);
    else if (cand->D1==-1) barePart.bits = static_cast<unsigned>(ParticleStatus::kStable);
    else                   barePart.bits = static_cast<unsigned>(ParticleStatus::kDecayed);
  }

  return StatusCode::SUCCESS;
}

