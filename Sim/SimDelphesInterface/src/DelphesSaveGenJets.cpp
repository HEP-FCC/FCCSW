#include "DelphesSaveGenJets.h"

// Delphes
#include "classes/DelphesClasses.h"
#include "modules/Delphes.h"

// datamodel
#include "datamodel/GenJetCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/TaggedGenJetCollection.h"

// ROOT
#include "TObjArray.h"

DECLARE_TOOL_FACTORY(DelphesSaveGenJets)

DelphesSaveGenJets::DelphesSaveGenJets(const std::string& aType, const std::string& aName, const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IDelphesSaveOutputTool>(this);
  declareProperty("genJets", m_genJets, "Handle to the jets to be saved");
  declareProperty(
      "genJetsFlavorTagged", m_taggedGenJets, "Handle to the jets including additional flavour tag information");
}

DelphesSaveGenJets::~DelphesSaveGenJets() {}

StatusCode DelphesSaveGenJets::initialize() { return GaudiTool::initialize(); }

StatusCode DelphesSaveGenJets::finalize() { return GaudiTool::finalize(); }

StatusCode DelphesSaveGenJets::saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) {
  // Create the collections
  auto colGenJets = m_genJets.createAndPut();
  auto colTaggedJets = m_taggedGenJets.createAndPut();

  const TObjArray* delphesColl = delphes.ImportArray(m_delphesArrayName.value().c_str());
  if (delphesColl == nullptr) {
    warning() << "Delphes collection " << m_delphesArrayName << " not present. Skipping it." << endmsg;
    return StatusCode::SUCCESS;
  }

  for (int j = 0; j < delphesColl->GetEntries(); ++j) {

    auto cand = static_cast<Candidate*>(delphesColl->At(j));

    // Jet info
    auto jet = colGenJets->create();
    auto bareJet = fcc::BareJet();
    bareJet.area = -1;
    bareJet.p4.px = cand->Momentum.Px();
    bareJet.p4.py = cand->Momentum.Py();
    bareJet.p4.pz = cand->Momentum.Pz();
    bareJet.p4.mass = cand->Momentum.M();
    jet.core(bareJet);

    // Flavor-tag info
    auto flavorGenJet = colTaggedJets->create();
    flavorGenJet.tag(cand->Flavor);
    flavorGenJet.jet(jet);

    // Debug: print FCC-EDM jets info
    if (msgLevel() <= MSG::DEBUG) {

      double energy = sqrt(jet.p4().px * jet.p4().px + jet.p4().py * jet.p4().py + jet.p4().pz * jet.p4().pz +
                           jet.p4().mass * jet.p4().mass);

      debug() << "Gen Jet: "
              << " Id: " << std::setw(3) << j + 1 << " Flavor: " << std::setw(3) << flavorGenJet.tag()
              << std::scientific << " Px: " << std::setprecision(2) << std::setw(9) << jet.p4().px
              << " Py: " << std::setprecision(2) << std::setw(9) << jet.p4().py << " Pz: " << std::setprecision(2)
              << std::setw(9) << jet.p4().pz << " E: " << std::setprecision(2) << std::setw(9) << energy
              << " M: " << std::setprecision(2) << std::setw(9) << jet.p4().mass << std::fixed << std::endl;
    }

    // Reference to MC - Delphes holds references to all objects related to the Jet object,
    // several relations might exist -> find "recursively" in a tree history the MC particle.
    // Add index to the reference index field to avoid double counting
    std::set<int> idRefMCPart;  // Avoid double counting when referencingh MC particles

    // Recursive procedure stops after the relation found is the one to MC particle and not to
    // a particle object. If particle not related to MC particle (<0 value)
    findJetPartMC(cand, mcParticles.size(), idRefMCPart);

    // Debug: print variable
    double totSimE = 0;

    for (auto id : idRefMCPart) {
      auto& mcParticle = mcParticles.at(id);
      jet.addparticles(mcParticles.at(id));

      // Debug: print FCC-EDM jet relation info
      if (msgLevel() <= MSG::DEBUG) {
        double recE = sqrt(jet.p4().px * jet.p4().px + jet.p4().py * jet.p4().py + jet.p4().pz * jet.p4().pz +
                           jet.p4().mass * jet.p4().mass);
        double simE = sqrt(mcParticle.p4().px * mcParticle.p4().px + mcParticle.p4().py * mcParticle.p4().py +
                           mcParticle.p4().pz * mcParticle.p4().pz + mcParticle.p4().mass * mcParticle.p4().mass);
        totSimE += simE;
        debug() << " RefId: " << std::setw(3) << id + 1 << " Rel E: " << std::setprecision(2) << std::scientific
                << std::setw(9) << simE << " " << std::setw(9) << totSimE << " <-> " << std::setw(9) << recE
                << std::fixed << std::endl;
      }  // Debug
    }

    // Debug: print end-line
    if (msgLevel() <= MSG::DEBUG) debug() << endmsg;
  }  // For - jets
  return StatusCode::SUCCESS;
}

void DelphesSaveGenJets::findJetPartMC(Candidate* cand, int rangeMCPart, std::set<int>& idRefMCPart) {
  // Warning - no MC relation found
  if (cand->GetCandidates()->GetEntries() == 0) {

    warning() << "Can't build one of the relations from Jet to MC particle!" << std::endl;
  }
  // Relation can be found
  else {

    for (auto itCand = cand->GetCandidates()->begin(); itCand != cand->GetCandidates()->end(); ++itCand) {

      Candidate* refCand = static_cast<Candidate*>(*itCand);
      int id = refCand->GetUniqueID() - 1;

      // std::cout << "Depth: " << depth << " " << id << std::endl;
      // Relation found
      if (id < rangeMCPart) {
        // std::cout << ">>> " << id << std::endl;
        idRefMCPart.insert(id);
      }
      // Not found -> step one level below
      else
        findJetPartMC(refCand, rangeMCPart, idRefMCPart);
    }
  }
}
