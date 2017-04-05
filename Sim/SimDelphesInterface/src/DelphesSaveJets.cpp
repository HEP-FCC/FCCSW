#include "DelphesSaveJets.h"

// Delphes
#include "classes/DelphesClasses.h"
#include "modules/Delphes.h"

// datamodel
#include "datamodel/JetCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/TaggedJetCollection.h"
// ROOT
#include "TObjArray.h"

DECLARE_TOOL_FACTORY(DelphesSaveJets)

DelphesSaveJets::DelphesSaveJets(const std::string& aType, const std::string& aName, const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IDelphesSaveOutputTool>(this);
  declareProperty("jets", m_jets, "Handle to the jets to be saved");
  declareProperty("jetConstituents", m_jetParticles, "Handle to the jet constituents to be saved");
  declareProperty("jetsFlavorTagged", m_jetsFlavorTagged, "Handle to the jet flavor tags to be saved");
  declareProperty("jetsBTagged", m_jetsBTagged, "Handle to the b tags to be saved");
  declareProperty("jetsCTagged", m_jetsCTagged, "Handle to the c tags to be saved");
  declareProperty("jetsTauTagged", m_jetsTauTagged, "Handle to the tau tags to be saved");
}

DelphesSaveJets::~DelphesSaveJets() {}

StatusCode DelphesSaveJets::initialize() { return GaudiTool::initialize(); }

StatusCode DelphesSaveJets::finalize() { return GaudiTool::finalize(); }

StatusCode DelphesSaveJets::saveOutput(Delphes& delphes, const fcc::MCParticleCollection& /*mcParticles*/) {
  // Create the collections
  auto colJets = m_jets.createAndPut();
  auto colJetsFlavor = m_jetsFlavorTagged.createAndPut();
  auto colBTags = m_jetsBTagged.createAndPut();
  auto colCTags = m_jetsCTagged.createAndPut();
  auto colTauTags = m_jetsTauTagged.createAndPut();
  auto colJetParts = m_jetParticles.createAndPut();

  const TObjArray* delphesColl = delphes.ImportArray(m_delphesArrayName.value().c_str());
  if (delphesColl == nullptr) {
    warning() << "Delphes collection " << m_delphesArrayName << " not present. Skipping it." << endmsg;
    return StatusCode::SUCCESS;
  }

  // Reference array of Jet constituents id in Delphes & in FCC EDM to avoid saving objects several-times
  std::map<int, int> refIDDelphIDFCC;  // ID Delphes, ID FCC EDM

  for (int j = 0; j < delphesColl->GetEntries(); ++j) {

    auto cand = static_cast<Candidate*>(delphesColl->At(j));

    // Jet info
    auto jet = colJets->create();
    auto bareJet = fcc::BareJet();
    bareJet.area = -1;
    bareJet.p4.px = cand->Momentum.Px();
    bareJet.p4.py = cand->Momentum.Py();
    bareJet.p4.pz = cand->Momentum.Pz();
    bareJet.p4.mass = cand->Momentum.M();
    jet.core(bareJet);

    // Flavor-tag info
    auto flavorTag = colJetsFlavor->create();
    flavorTag.tag(cand->Flavor);
    flavorTag.jet(jet);

    // B-tag info
    auto bTag = colBTags->create();
    bTag.tag(cand->BTag & (1 << 0));  // btagging is stored in bit 0 of BTag variable
    bTag.jet(jet);

    // C-tag info
    auto cTag = colCTags->create();
    cTag.tag(cand->BTag & (1 << 1));  // ctagging is stored in bit 1 of BTag variable
    cTag.jet(jet);

    // Tau-tag info
    auto tauTag = colTauTags->create();
    tauTag.tag(cand->TauTag);
    tauTag.jet(jet);

    // Flavour-tag info

    // Debug: print FCC-EDM jets info
    if (msgLevel() <= MSG::DEBUG) {

      double energy = sqrt(jet.p4().px * jet.p4().px + jet.p4().py * jet.p4().py + jet.p4().pz * jet.p4().pz +
                           jet.p4().mass * jet.p4().mass);

      debug() << "Jet: "
              << " Id: " << std::setw(3) << j + 1 << " Flavor: " << std::setw(3) << flavorTag.tag()
              << " BTag: " << std::setprecision(1) << std::setw(3) << bTag.tag() << " CTag: " << std::setprecision(1)
              << std::setw(3) << cTag.tag() << " TauTag: " << std::setprecision(1) << std::setw(3) << tauTag.tag()
              << std::scientific << " Px: " << std::setprecision(2) << std::setw(9) << jet.p4().px
              << " Py: " << std::setprecision(2) << std::setw(9) << jet.p4().py << " Pz: " << std::setprecision(2)
              << std::setw(9) << jet.p4().pz << " E: " << std::setprecision(2) << std::setw(9) << energy
              << " M: " << std::setprecision(2) << std::setw(9) << jet.p4().mass << std::fixed << std::endl;
    }

    // Get corresponding jet constituents
    for (auto itCand = cand->GetCandidates()->begin(); itCand != cand->GetCandidates()->end(); ++itCand) {

      // Jet constituent
      Candidate* candJetPart = static_cast<Candidate*>(*itCand);

      auto itRefID = refIDDelphIDFCC.find(candJetPart->GetUniqueID());

      // Jet part already saved in collection --> make only relation
      if (itRefID != refIDDelphIDFCC.end()) {

        auto index = itRefID->second;

        if (index >= 0) {
          auto jetPart = colJetParts->at(index);
          jet.addparticles(jetPart);

          debug() << "  Constituent Old - idxid: " << candJetPart->GetUniqueID() << " " << jetPart.getObjectID().index
                  << std::setprecision(2) << std::scientific << " Px: " << std::setw(9) << jetPart.p4().px
                  << " Py: " << std::setw(9) << jetPart.p4().py << " Pz: " << std::setw(9) << jetPart.p4().pz
                  << " M: " << std::setw(9) << jetPart.p4().mass << std::fixed << std::endl;
        } else {
          error() << "Algorithm assigning jet constituents failed - check" << endmsg;
        }
      }
      // Jet part not found --> create it + make relation
      else {

        auto jetPart = colJetParts->create();
        auto barePart = fcc::BareParticle();
        barePart.pdgId = 0;
        barePart.status = 0;
        barePart.p4.px = candJetPart->Momentum.Px();
        barePart.p4.py = candJetPart->Momentum.Py();
        barePart.p4.pz = candJetPart->Momentum.Pz();
        barePart.p4.mass = candJetPart->Momentum.M();
        barePart.charge = candJetPart->Charge;
        barePart.vertex.x = 0;
        barePart.vertex.y = 0;
        barePart.vertex.z = 0;
        jetPart.core(barePart);

        jet.addparticles(jetPart);

        // Assign fcc constituent id to delphes constituent id
        refIDDelphIDFCC[candJetPart->GetUniqueID()] = jetPart.getObjectID().index;

        debug() << "  Constituent New - idxid: " << candJetPart->GetUniqueID() << " " << jetPart.getObjectID().index
                << std::setprecision(2) << std::scientific << " Px: " << std::setw(9) << jetPart.p4().px
                << " Py: " << std::setw(9) << jetPart.p4().py << " Pz: " << std::setw(9) << jetPart.p4().pz
                << " M: " << std::setw(9) << jetPart.p4().mass << std::fixed << std::endl;
      }

    }  // Jet constituents

    // Debug: print end-line
    if (msgLevel() <= MSG::DEBUG) debug() << endmsg;

  }  // For - jets
  return StatusCode::SUCCESS;
}
