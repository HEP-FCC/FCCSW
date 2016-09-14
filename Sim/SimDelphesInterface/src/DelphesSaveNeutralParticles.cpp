#include "DelphesSaveNeutralParticles.h"

// Delphes
#include "modules/Delphes.h"
#include "classes/DelphesClasses.h"

// datamodel
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"
#include "datamodel/TagCollection.h"
#include "datamodel/ParticleTagAssociationCollection.h"
#include "datamodel/MCParticleCollection.h"
// ROOT
#include "TObjArray.h"
DECLARE_TOOL_FACTORY(DelphesSaveNeutralParticles)

DelphesSaveNeutralParticles::DelphesSaveNeutralParticles(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  GaudiTool(aType, aName, aParent) {
  declareInterface<IDelphesSaveOutputTool>(this);
  declareOutput("particles", m_particles);
  declareOutput("mcAssociations", m_mcAssociations);
  declareOutput("isolationTags", m_isolationTags);
  declareOutput("isolationAssociations", m_isoAssociations);
  declareProperty("delphesArrayName", m_delphesArrayName);
  declareProperty("saveIsolation", m_saveIso=true);
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

DelphesSaveNeutralParticles::~DelphesSaveNeutralParticles() {}

StatusCode DelphesSaveNeutralParticles::initialize() {
  return GaudiTool::initialize();
}

StatusCode DelphesSaveNeutralParticles::finalize() {
  return GaudiTool::finalize();
}

StatusCode DelphesSaveNeutralParticles::saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) {
  // Create the collections

  auto colParticles = m_particles.createAndPut();
  auto ascColParticlesToMC = m_mcAssociations.createAndPut();
  fcc::TagCollection* colITags(nullptr);
  fcc::ParticleTagAssociationCollection* ascColParticlesToITags(nullptr);
  if (m_saveIso) {
    colITags = m_isolationTags.createAndPut();
    ascColParticlesToITags = m_isoAssociations.createAndPut();
  }

  const TObjArray* delphesColl = delphes.ImportArray(m_delphesArrayName.c_str());
  if (delphesColl == nullptr) {
    warning() << "Delphes collection " << m_delphesArrayName << " not present. Skipping it." << endmsg;
    return StatusCode::SUCCESS;
  }
  for(int j=0; j<delphesColl->GetEntries(); j++) {

    auto cand     = static_cast<Candidate *>(delphesColl->At(j));
    auto particle = colParticles->create();

    auto barePart     = fcc::BareParticle();
    barePart.Type     = cand->PID;
    barePart.Status   = cand->Status;
    barePart.P4.Px    = cand->Momentum.Px();
    barePart.P4.Py    = cand->Momentum.Py();
    barePart.P4.Pz    = cand->Momentum.Pz();
    barePart.P4.Mass  = cand->Momentum.M();
    barePart.Charge   = cand->Charge;
    barePart.Vertex.X = cand->Position.X();
    barePart.Vertex.Y = cand->Position.Y();
    barePart.Vertex.Z = cand->Position.Z();
    particle.Core(barePart);

    // Isolation-tag info
    float iTagValue = 0;
    if (colITags!=nullptr) {

      auto iTag           = colITags->create();
      auto relationToITag = ascColParticlesToITags->create();
      iTag.Value(cand->IsolationVar);
      relationToITag.Particle(particle);
      relationToITag.Tag(iTag);

      iTagValue = iTag.Value();
    }

    // Debug: print FCC-EDM tower info
    if (msgLevel() <= MSG::DEBUG) {

      double energy = sqrt(particle.Core().P4.Px*particle.Core().P4.Px +
                           particle.Core().P4.Py*particle.Core().P4.Py +
                           particle.Core().P4.Pz*particle.Core().P4.Pz +
                           particle.Core().P4.Mass*particle.Core().P4.Mass);

      debug() << "Tower: "
              << " Id: "       << std::setw(3) << j+1
              << " Pdg: "      << std::setw(5) << particle.Core().Type
              << " Stat: "     << std::setw(2) << particle.Core().Status
              << " Bits: "     << std::setw(2) << particle.Core().Bits;

      if (colITags!=nullptr) {

        debug() << " ITag: "   << std::setprecision(1) << std::setw(4) <<  iTagValue;
      }

      debug() << std::scientific
              << " Px: "       << std::setprecision(2) << std::setw(9) << particle.Core().P4.Px
              << " Py: "       << std::setprecision(2) << std::setw(9) << particle.Core().P4.Py
              << " Pz: "       << std::setprecision(2) << std::setw(9) << particle.Core().P4.Pz
              << " E: "        << std::setprecision(2) << std::setw(9) << energy
              << " M: "        << std::setprecision(2) << std::setw(9) << particle.Core().P4.Mass
              << " Vx: "       << std::setprecision(2) << std::setw(9) << particle.Core().Vertex.X
              << " Vy: "       << std::setprecision(2) << std::setw(9) << particle.Core().Vertex.Y
              << " Vz: "       << std::setprecision(2) << std::setw(9) << particle.Core().Vertex.Z
              << std::fixed
              << std::endl;
    } // Debug

    // Reference to MC - Delphes holds references to all objects related to the Photon object, several relations might exist for gammas
    std::set<int> idRefMCPart; // Avoid double counting when referencingh MC particles

    // Get corresponding cluster in calorimeter
    for (auto itCand=cand->GetCandidates()->begin(); itCand!=cand->GetCandidates()->end(); ++itCand) {

      // Cluster in calorimeter
      Candidate* clsCand = static_cast<Candidate*>(*itCand);

      // Get corresponding MC particle
      for (auto itCls=clsCand->GetCandidates()->begin(); itCls!=clsCand->GetCandidates()->end(); ++itCls) {

        Candidate* refCand = static_cast<Candidate*>(*itCls);
        int id = refCand->GetUniqueID()-1;
        if (id<mcParticles.size()) idRefMCPart.insert(id);
        else {
          warning() << "Can't build one of the relations from Photon/NHadron to MC particle!" << std::endl;
        }

      } // Iter MC particles
    } // Iter cluster

    // Debug: print variable
    double totSimE = 0;

    // Save relations
    for (auto id : idRefMCPart) {

      auto relation = ascColParticlesToMC->create();
      relation.Rec(particle);
      relation.Sim(mcParticles.at(id));

      // Debug: print FCC-EDM tower relation info
      if (msgLevel() <= MSG::DEBUG) {
        double recE   = sqrt(relation.Rec().Core().P4.Px*relation.Rec().Core().P4.Px +
                             relation.Rec().Core().P4.Py*relation.Rec().Core().P4.Py +
                             relation.Rec().Core().P4.Pz*relation.Rec().Core().P4.Pz +
                             relation.Rec().Core().P4.Mass*relation.Rec().Core().P4.Mass);
        double simE   = sqrt(relation.Sim().Core().P4.Px*relation.Sim().Core().P4.Px +
                             relation.Sim().Core().P4.Py*relation.Sim().Core().P4.Py +
                             relation.Sim().Core().P4.Pz*relation.Sim().Core().P4.Pz +
                             relation.Sim().Core().P4.Mass*relation.Sim().Core().P4.Mass);

        totSimE += simE;
        debug() << " RefId: " << std::setw(3)            << id+1
                << " Rel E: " << std::setprecision(2)
                              << std::scientific
                              << std::setw(9) << simE    << " "
                              << std::setw(9) << totSimE << " <-> "
                              << std::setw(9) << recE
                              << std::fixed;
        if      (mcParticles.at(id).Core().Type ==22) debug() << " Gamma";
        else if (mcParticles.at(id).Core().Charge==0) debug() << " Neutral";
        debug() << std::endl;
      } // Debug
    }

    // Debug: print end-line
    if (msgLevel() <= MSG::DEBUG) debug() << endmsg;
  } // For - towers
  return StatusCode::SUCCESS;
}
