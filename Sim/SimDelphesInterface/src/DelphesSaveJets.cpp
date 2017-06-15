#include "DelphesSaveJets.h"

// Delphes
#include "classes/DelphesClasses.h"
#include "modules/Delphes.h"

// datamodel
#include "datamodel/JetCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/TaggedJetCollection.h"
#include "datamodel/ResolvedJetCollection.h"
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

  // --- jet substructure variables ----
  declareProperty("jetsOneSubJettinessTagged", m_jetsOneSubJettinessTagged, "Handle to the 1-subjettiness tag");
  declareProperty("jetsTwoSubJettinessTagged", m_jetsTwoSubJettinessTagged, "Handle to the 2-subjettiness tag");
  declareProperty("jetsThreeSubJettinessTagged", m_jetsThreeSubJettinessTagged, "Handle to the 3-subjettiness tag");
  declareProperty("subjetsTrimmingTagged", m_subjetsTrimmingTagged, "Handle to the trimmed subjets tag");
  declareProperty("subjetsPruningTagged", m_subjetsPruningTagged, "Handle to the pruned subjets tag");
  declareProperty("subjetsSoftDropTagged", m_subjetsSoftDropTagged, "Handle to the soft dropped subjets tag");
  declareProperty("subjetsTrimming", m_subjetsTrimming, "Handle to the trimmed subjets collection");
  declareProperty("subjetsPruning", m_subjetsPruning, "Handle to the pruned subjets collection");
  declareProperty("subjetsSoftDrop", m_subjetsSoftDrop, "Handle to the soft dropped subjets collection");
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

  // --- jet substructure variables ----
  fcc::TaggedJetCollection* colOneSubjettinessTags(nullptr);
  fcc::TaggedJetCollection* colTwoSubjettinessTags(nullptr);
  fcc::TaggedJetCollection* colThreeSubjettinessTags(nullptr);
  fcc::ResolvedJetCollection* colSubjetsTrimmingTags(nullptr);
  fcc::ResolvedJetCollection* colSubjetsPruningTags(nullptr);
  fcc::ResolvedJetCollection* colSubjetsSoftDropTags(nullptr);
  fcc::JetCollection* colSubjetsTrimming(nullptr);
  fcc::JetCollection* colSubjetsPruning(nullptr);
  fcc::JetCollection* colSubjetsSoftDrop(nullptr);  
  if (m_saveSubstructure) {
    colOneSubjettinessTags = m_jetsOneSubJettinessTagged.createAndPut();
    colTwoSubjettinessTags = m_jetsTwoSubJettinessTagged.createAndPut();
    colThreeSubjettinessTags = m_jetsThreeSubJettinessTagged.createAndPut();
    colSubjetsTrimmingTags = m_subjetsTrimmingTagged.createAndPut();
    colSubjetsPruningTags = m_subjetsPruningTagged.createAndPut();
    colSubjetsSoftDropTags = m_subjetsSoftDropTagged.createAndPut();
    colSubjetsTrimming = m_subjetsTrimming.createAndPut();
    colSubjetsPruning = m_subjetsPruning.createAndPut();
    colSubjetsSoftDrop = m_subjetsSoftDrop.createAndPut();
 }

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

    // --- Jet Substructure info ---

    if (m_saveSubstructure) {
    
      // 1-subjettiness
      auto oneSubjTag = colOneSubjettinessTags->create();
      oneSubjTag.tag(cand->Tau[0]);
      oneSubjTag.jet(jet);

      // 2-subjettiness
      auto twoSubjTag = colTwoSubjettinessTags->create();
      twoSubjTag.tag(cand->Tau[1]);
      twoSubjTag.jet(jet);

      // 3-subjettiness
      auto threeSubjTag = colThreeSubjettinessTags->create();
      threeSubjTag.tag(cand->Tau[2]);
      threeSubjTag.jet(jet);

      // Get sub-jets (up to 5 subjets are stored in Delphes candidates, the first entry always corresponds to the cleaned fat-jet)
    
      // trimming
      auto subjetsTrimmingTag = colSubjetsTrimmingTags->create();
      subjetsTrimmingTag.jet(jet);
      for (size_t i = 0; i < 5; i++) {
        if (cand->TrimmedP4[i].Pt() > 0) {
          auto subjet = colSubjetsTrimming->create();
          auto bareSubjet = fcc::BareJet();
          bareSubjet.area = -1;
          bareSubjet.p4.px = cand->TrimmedP4[i].Px();
          bareSubjet.p4.py = cand->TrimmedP4[i].Py();
          bareSubjet.p4.pz = cand->TrimmedP4[i].Pz();
          bareSubjet.p4.mass = cand->TrimmedP4[i].M();
          subjet.core(bareSubjet);
          subjetsTrimmingTag.addsubjets(subjet);
        }
      }

      // pruning
      auto subjetsPruningTag = colSubjetsPruningTags->create();
      subjetsPruningTag.jet(jet);
      for (size_t i = 0; i < 5; i++) {
        if (cand->PrunedP4[i].Pt() > 0) {
          auto subjet = colSubjetsPruning->create();
          auto bareSubjet = fcc::BareJet();
          bareSubjet.area = -1;
          bareSubjet.p4.px = cand->PrunedP4[i].Px();
          bareSubjet.p4.py = cand->PrunedP4[i].Py();
          bareSubjet.p4.pz = cand->PrunedP4[i].Pz();
          bareSubjet.p4.mass = cand->PrunedP4[i].M();
          subjet.core(bareSubjet);
          subjetsPruningTag.addsubjets(subjet);
        }
      }

      // soft drop
      auto subjetsSoftDropTag = colSubjetsSoftDropTags->create();
      subjetsSoftDropTag.jet(jet);
      for (size_t i = 0; i < 5; i++) {
        if (cand->SoftDroppedP4[i].Pt() > 0) {
          auto subjet = colSubjetsSoftDrop->create();
          auto bareSubjet = fcc::BareJet();
          bareSubjet.area = -1;
          bareSubjet.p4.px = cand->SoftDroppedP4[i].Px();
          bareSubjet.p4.py = cand->SoftDroppedP4[i].Py();
          bareSubjet.p4.pz = cand->SoftDroppedP4[i].Pz();
          bareSubjet.p4.mass = cand->SoftDroppedP4[i].M();
          subjet.core(bareSubjet);
          subjetsSoftDropTag.addsubjets(subjet);
        }
      }
    } // end jet substructure

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
