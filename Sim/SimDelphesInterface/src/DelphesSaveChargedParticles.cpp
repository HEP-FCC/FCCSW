#include "DelphesSaveChargedParticles.h"

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
#include "SimDelphesInterface/ParticleStatus.h"

DECLARE_TOOL_FACTORY(DelphesSaveChargedParticles)

DelphesSaveChargedParticles::DelphesSaveChargedParticles(const std::string& aType, const std::string& aName, const IInterface* aParent) :
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

DelphesSaveChargedParticles::~DelphesSaveChargedParticles() {}

StatusCode DelphesSaveChargedParticles::initialize() {
  return GaudiTool::initialize();
}

StatusCode DelphesSaveChargedParticles::finalize() {
  return GaudiTool::finalize();
}

StatusCode DelphesSaveChargedParticles::saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) {
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

    // Reference to MC - Delphes holds references to all objects related to the <T> object, only one relates to MC particle
    auto relation   = ascColParticlesToMC->create();
    int idRefMCPart = -1;
    if (cand->GetCandidates()->GetEntries()>0) {

      auto refCand = static_cast<Candidate*>(cand->GetCandidates()->At(0));
      idRefMCPart  = refCand->GetUniqueID()-1;     // Use C numbering from 0
      if (idRefMCPart<mcParticles.size()) {

        barePart.Bits = static_cast<unsigned>(ParticleStatus::kMatched);
        particle.Core(barePart);
        relation.Rec(particle);
        relation.Sim(mcParticles.at(idRefMCPart));
      }
      else {
        barePart.Bits = static_cast<unsigned>(ParticleStatus::kUnmatched);
        particle.Core(barePart);
        warning() << "Can't build relation from Electron/Muon/ChHadron to MC particle!" << std::endl;
      }
    }
    else {
      barePart.Bits = static_cast<unsigned>(ParticleStatus::kUnmatched);
      particle.Core(barePart);
      warning() << "Can't build relation from Electron/Muon/ChHadron to MC particle!" << std::endl;
    }

    // Debug: print FCC-EDM track info
    if (msgLevel() <= MSG::DEBUG) {

      double energy = sqrt(particle.Core().P4.Px*particle.Core().P4.Px +
                           particle.Core().P4.Py*particle.Core().P4.Py +
                           particle.Core().P4.Pz*particle.Core().P4.Pz +
                           particle.Core().P4.Mass*particle.Core().P4.Mass);
      double recE   = sqrt(relation.Rec().Core().P4.Px*relation.Rec().Core().P4.Px +
                           relation.Rec().Core().P4.Py*relation.Rec().Core().P4.Py +
                           relation.Rec().Core().P4.Pz*relation.Rec().Core().P4.Pz +
                           relation.Rec().Core().P4.Mass*relation.Rec().Core().P4.Mass);
      double simE   = sqrt(relation.Sim().Core().P4.Px*relation.Sim().Core().P4.Px +
                           relation.Sim().Core().P4.Py*relation.Sim().Core().P4.Py +
                           relation.Sim().Core().P4.Pz*relation.Sim().Core().P4.Pz +
                           relation.Sim().Core().P4.Mass*relation.Sim().Core().P4.Mass);

      debug() << "Track: "
              << " Id: "       << std::setw(3)  << j+1
              << " Pdg: "      << std::setw(5)  << particle.Core().Type
              << " Stat: "     << std::setw(2)  << particle.Core().Status
              << " Bits: "     << std::setw(2)  << particle.Core().Bits;

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
              << " RefId: "    << std::setw(3)  << idRefMCPart+1
              << " Rel E: "    << std::setprecision(2) << std::setw(9) << simE << " <-> " << std::setw(9) << recE
              << std::fixed
              << endmsg;
    } // Debug
  } // For - tracks
  return StatusCode::SUCCESS;
}
