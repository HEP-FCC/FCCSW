#include "DelphesSaveChargedParticles.h"

// Delphes
#include "classes/DelphesClasses.h"
#include "modules/Delphes.h"

// datamodel
#include "datamodel/MCParticleCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"
#include "datamodel/TaggedParticleCollection.h"
// ROOT
#include "SimDelphesInterface/ParticleStatus.h"
#include "TObjArray.h"

DECLARE_TOOL_FACTORY(DelphesSaveChargedParticles)

DelphesSaveChargedParticles::DelphesSaveChargedParticles(const std::string& aType,
                                                         const std::string& aName,
                                                         const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IDelphesSaveOutputTool>(this);
  declareProperty("particles", m_particles, "Handle the particles to be saved");
  declareProperty("mcAssociations", m_mcAssociations, "Handle to associate particles with MCParticles");
  declareProperty("isolationTags", m_isolationTaggedParticles, "Handle for isolation tags");
}

DelphesSaveChargedParticles::~DelphesSaveChargedParticles() {}

StatusCode DelphesSaveChargedParticles::initialize() { return GaudiTool::initialize(); }

StatusCode DelphesSaveChargedParticles::finalize() { return GaudiTool::finalize(); }

StatusCode DelphesSaveChargedParticles::saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) {
  // Create the collections
  auto colParticles = m_particles.createAndPut();
  auto ascColParticlesToMC = m_mcAssociations.createAndPut();

  fcc::TaggedParticleCollection* colITags(nullptr);
  if (m_saveIso) {
    colITags = m_isolationTaggedParticles.createAndPut();
  }

  const TObjArray* delphesColl = delphes.ImportArray(m_delphesArrayName.value().c_str());
  if (delphesColl == nullptr) {
    warning() << "Delphes collection " << m_delphesArrayName << " not present. Skipping it." << endmsg;
    return StatusCode::SUCCESS;
  }
  for (int j = 0; j < delphesColl->GetEntries(); j++) {
    auto cand = static_cast<Candidate*>(delphesColl->At(j));
    auto particle = colParticles->create();

    auto& barePart = particle.core();
    barePart.pdgId = cand->PID;
    barePart.status = cand->Status;
    barePart.p4.px = cand->Momentum.Px();
    barePart.p4.py = cand->Momentum.Py();
    barePart.p4.pz = cand->Momentum.Pz();
    barePart.p4.mass = cand->Momentum.M();
    barePart.charge = cand->Charge;
    barePart.vertex.x = cand->InitialPosition.X();
    barePart.vertex.y = cand->InitialPosition.Y();
    barePart.vertex.z = cand->InitialPosition.Z();

    // Isolation-tag info
    float iTagValue = 0;
    if (colITags != nullptr) {
      auto iTag = colITags->create();
      iTag.tag(cand->IsolationVar);
      iTag.particle(particle);
      iTagValue = iTag.tag();
    }

    // Reference to MC - Delphes holds references to all objects related to the <T> object, only one relates to MC
    // particle
    auto relation = ascColParticlesToMC->create();
    if (cand->GetCandidates()->GetEntries() > 0) {
      auto refCand = static_cast<Candidate*>(cand->GetCandidates()->At(0));

      // find refCand in mcParticle collection
      int index = -1;
      for (int k = 0; k < mcParticles.size(); k++) {
        if (mcParticles.at(k).core().bits == refCand->GetUniqueID()) {
          index = k;
          break;
        }
      }

      if (index > 0) {
        barePart.bits = static_cast<unsigned>(ParticleStatus::kMatched);
        relation.rec(particle);
        relation.sim(mcParticles.at(index));
      } else {
        barePart.bits = static_cast<unsigned>(ParticleStatus::kUnmatched);
        warning() << "Can't build relation from Electron/Muon/ChHadron to MC particle!" << std::endl;
      }
    } else {
      barePart.bits = static_cast<unsigned>(ParticleStatus::kUnmatched);
      warning() << "Can't build relation from Electron/Muon/ChHadron to MC particle!" << std::endl;
    }

    // Debug: print FCC-EDM track info
    if (msgLevel() <= MSG::DEBUG) {

      double energy = sqrt(particle.p4().px * particle.p4().px + particle.p4().py * particle.p4().py +
                           particle.p4().pz * particle.p4().pz + particle.p4().mass * particle.p4().mass);
      double recE =
          sqrt(relation.rec().p4().px * relation.rec().p4().px + relation.rec().p4().py * relation.rec().p4().py +
               relation.rec().p4().pz * relation.rec().p4().pz + relation.rec().p4().mass * relation.rec().p4().mass);
      double simE =
          sqrt(relation.sim().p4().px * relation.sim().p4().px + relation.sim().p4().py * relation.sim().p4().py +
               relation.sim().p4().pz * relation.sim().p4().pz + relation.sim().p4().mass * relation.sim().p4().mass);

      debug() << "Track: "
              << " Id: " << std::setw(3) << j + 1 << " Pdg: " << std::setw(5) << particle.pdgId()
              << " Stat: " << std::setw(2) << particle.status() << " Bits: " << std::setw(2) << particle.bits();

      if (colITags != nullptr) {

        debug() << " ITag: " << std::setprecision(1) << std::setw(4) << iTagValue;
      }

      debug() << std::scientific << " Px: " << std::setprecision(2) << std::setw(9) << particle.p4().px
              << " Py: " << std::setprecision(2) << std::setw(9) << particle.p4().py << " Pz: " << std::setprecision(2)
              << std::setw(9) << particle.p4().pz << " E: " << std::setprecision(2) << std::setw(9) << energy
              << " M: " << std::setprecision(2) << std::setw(9) << particle.p4().mass << " Vx: " << std::setprecision(2)
              << std::setw(9) << particle.vertex().x << " Vy: " << std::setprecision(2) << std::setw(9)
              << particle.vertex().y << " Vz: " << std::setprecision(2) << std::setw(9) << particle.vertex().z
              << " Rel E: " << std::setprecision(2) << std::setw(9) << simE << " <-> " << std::setw(9) << recE
              << std::fixed << endmsg;
    }  // Debug
  }    // For - tracks
  return StatusCode::SUCCESS;
}
