#include "DelphesSaveNeutralParticles.h"

// Delphes
#include "classes/DelphesClasses.h"
#include "modules/Delphes.h"

// datamodel
#include "datamodel/MCParticleCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"
#include "datamodel/TaggedParticleCollection.h"
// ROOT
#include "TObjArray.h"
DECLARE_TOOL_FACTORY(DelphesSaveNeutralParticles)

DelphesSaveNeutralParticles::DelphesSaveNeutralParticles(const std::string& aType,
                                                         const std::string& aName,
                                                         const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IDelphesSaveOutputTool>(this);
  declareProperty("particles", m_particles, "Handle the particles to be saved");
  declareProperty("mcAssociations", m_mcAssociations, "Handle to associate particles with MCParticles");
  declareProperty("isolationTags", m_isolationTags, "Handle for particles with isolation information");
}

DelphesSaveNeutralParticles::~DelphesSaveNeutralParticles() {}

StatusCode DelphesSaveNeutralParticles::initialize() { return GaudiTool::initialize(); }

StatusCode DelphesSaveNeutralParticles::finalize() { return GaudiTool::finalize(); }

StatusCode DelphesSaveNeutralParticles::saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) {
  // Create the collections

  auto colParticles = m_particles.createAndPut();
  auto ascColParticlesToMC = m_mcAssociations.createAndPut();
  fcc::TaggedParticleCollection* colITags(nullptr);
  if (m_saveIso) {
    colITags = m_isolationTags.createAndPut();
  }

  const TObjArray* delphesColl = delphes.ImportArray(m_delphesArrayName.value().c_str());
  if (delphesColl == nullptr) {
    warning() << "Delphes collection " << m_delphesArrayName << " not present. Skipping it." << endmsg;
    return StatusCode::SUCCESS;
  }

  for (int j = 0; j < delphesColl->GetEntries(); j++) {

    auto cand = static_cast<Candidate*>(delphesColl->At(j));
    auto particle = colParticles->create();

    auto barePart = fcc::BareParticle();
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
    particle.core(barePart);

    // Isolation-tag info
    float iTagValue = 0;
    if (colITags != nullptr) {
      auto iTag = colITags->create();
      iTag.tag(cand->IsolationVar);
      iTag.particle(particle);

      iTagValue = iTag.tag();
    }

    // Debug: print FCC-EDM tower info
    if (msgLevel() <= MSG::DEBUG) {

      double energy = sqrt(particle.p4().px * particle.p4().px + particle.p4().py * particle.p4().py +
                           particle.p4().pz * particle.p4().pz + particle.p4().mass * particle.p4().mass);

      debug() << "Tower: "
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
              << std::fixed << std::endl;
    }  // Debug

    // Reference to MC - Delphes holds references to all objects related to the Photon object, several relations might
    // exist for gammas
    std::set<int> idRefMCPart;  // Avoid double counting when referencingh MC particles
    // Loop over hits in a given tower
    for (auto itCand = cand->GetCandidates()->begin(); itCand != cand->GetCandidates()->end(); ++itCand) {
      Candidate* hit = static_cast<Candidate*>(*itCand);
      if (hit->GetCandidates()->GetEntries() > 0) {

        // Get MC particle
        Candidate* refCand = static_cast<Candidate*>(hit->GetCandidates()->At(0));
        for (int k = 0; k < mcParticles.size(); k++) {
          if (mcParticles.at(k).core().bits == refCand->GetUniqueID()) {
            idRefMCPart.insert(k);
            break;
          }
        }  // Iter MC particles
      }
    }  // Iter hits on tower

    // Debug: print variable
    double totSimE = 0;

    // Save relations
    for (auto id : idRefMCPart) {

      auto relation = ascColParticlesToMC->create();
      relation.rec(particle);
      relation.sim(mcParticles.at(id));

      // Debug: print FCC-EDM tower relation info
      if (msgLevel() <= MSG::DEBUG) {
        double recE =
            sqrt(relation.rec().p4().px * relation.rec().p4().px + relation.rec().p4().py * relation.rec().p4().py +
                 relation.rec().p4().pz * relation.rec().p4().pz + relation.rec().p4().mass * relation.rec().p4().mass);
        double simE =
            sqrt(relation.sim().p4().px * relation.sim().p4().px + relation.sim().p4().py * relation.sim().p4().py +
                 relation.sim().p4().pz * relation.sim().p4().pz + relation.sim().p4().mass * relation.sim().p4().mass);

        if (relation.sim().charge() != 0) totSimE += simE;
        debug() << " RefId: " << std::setw(3) << id + 1 << " Rel E: " << std::setprecision(2) << std::scientific
                << std::setw(9) << simE << " " << std::setw(9) << totSimE << " <-> " << std::setw(9) << recE
                << std::fixed;
        if (mcParticles.at(id).pdgId() == 22)
          debug() << " Gamma";
        else if (mcParticles.at(id).charge() == 0)
          debug() << " Neutral";
        debug() << std::endl;
      }  // Debug
    }

    // Debug: print end-line
    if (msgLevel() <= MSG::DEBUG) debug() << endmsg;
  }  // For - towers
  return StatusCode::SUCCESS;
}
