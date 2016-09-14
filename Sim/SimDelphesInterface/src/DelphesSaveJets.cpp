#include "DelphesSaveJets.h"

// Delphes
#include "modules/Delphes.h"
#include "classes/DelphesClasses.h"

// datamodel
#include "datamodel/JetCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/JetParticleAssociationCollection.h"
#include "datamodel/JetIntTagAssociationCollection.h"
#include "datamodel/TagCollection.h"
#include "datamodel/IntTagCollection.h"
#include "datamodel/JetTagAssociationCollection.h"
#include "datamodel/MCParticleCollection.h"
// ROOT
#include "TObjArray.h"

DECLARE_TOOL_FACTORY(DelphesSaveJets)

DelphesSaveJets::DelphesSaveJets(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  GaudiTool(aType, aName, aParent) {
  declareInterface<IDelphesSaveOutputTool>(this);

  declareOutput("jets", m_jets);
  declareOutput("jetConstituents", m_jetParticles);
  declareOutput("jetConstituentAssociations", m_jetParticleAssociations);
  declareOutput("jetFlavorTags", m_jetFlavorTags);
  declareOutput("jetFlavorTagAssociations", m_jetFlavorAssociations);
  declareOutput("bTags", m_bTags);
  declareOutput("jetBTagAssociations", m_jetBTagAssociations);
  declareOutput("cTags", m_cTags);
  declareOutput("jetCTagAssociations", m_jetCTagAssociations);
  declareOutput("tauTags", m_tauTags);
  declareOutput("jetTauTagAssociations", m_jetTauTagAssociations);
  declareProperty("delphesArrayName", m_delphesArrayName);
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

DelphesSaveJets::~DelphesSaveJets() {}

StatusCode DelphesSaveJets::initialize() {
  return GaudiTool::initialize();
}

StatusCode DelphesSaveJets::finalize() {
  return GaudiTool::finalize();
}

StatusCode DelphesSaveJets::saveOutput(Delphes& delphes, const fcc::MCParticleCollection& /*mcParticles*/) {
  // Create the collections
  auto colJets = m_jets.createAndPut();
  auto colJetsFlavor = m_jetFlavorTags.createAndPut();
  auto ascColJetsToFlavor = m_jetFlavorAssociations.createAndPut();
  auto colBTags = m_bTags.createAndPut();
  auto ascColJetsToBTags = m_jetBTagAssociations.createAndPut();
  auto colCTags = m_cTags.createAndPut();
  auto ascColJetsToCTags = m_jetCTagAssociations.createAndPut();
  auto colTauTags = m_tauTags.createAndPut();
  auto ascColJetsToTauTags = m_jetTauTagAssociations.createAndPut();
  auto colJetParts = m_jetParticles.createAndPut();
  auto ascColJetsToPart = m_jetParticleAssociations.createAndPut();


  const TObjArray* delphesColl = delphes.ImportArray(m_delphesArrayName.c_str());
  if (delphesColl == nullptr) {
    warning() << "Delphes collection " << m_delphesArrayName << " not present. Skipping it." << endmsg;
    return StatusCode::SUCCESS;
  }

  // Reference array of Jet constituents id in Delphes & in FCC EDM to avoid saving objects several-times
  std::map<int,int> refIDDelphIDFCC; // ID Delphes, ID FCC EDM

  for(int j = 0; j < delphesColl->GetEntries(); ++j) {

    auto cand = static_cast<Candidate *>(delphesColl->At(j));

    // Jet info
    auto jet         = colJets->create();
    auto bareJet     = fcc::BareJet();
    bareJet.Area     = -1;
    bareJet.P4.Px    = cand->Momentum.Px();
    bareJet.P4.Py    = cand->Momentum.Py();
    bareJet.P4.Pz    = cand->Momentum.Pz();
    bareJet.P4.Mass  = cand->Mass;
    jet.Core(bareJet);

    // Flavor-tag info
    auto flavorTag        = colJetsFlavor->create();
    auto relationToFlavor = ascColJetsToFlavor->create();
    flavorTag.Value(cand->Flavor);
    relationToFlavor.Jet(jet);
    relationToFlavor.Tag(flavorTag);

    // B-tag info
    auto bTag             = colBTags->create();
    auto relationToBTag   = ascColJetsToBTags->create();
    bTag.Value(cand->BTag & (1 << 0)); // btagging is stored in bit 0 of BTag variable
    relationToBTag.Jet(jet);
    relationToBTag.Tag(bTag);

    // C-tag info
    auto cTag             = colCTags->create();
    auto relationToCTag   = ascColJetsToCTags->create();
    cTag.Value(cand->BTag & (1 << 1)); // ctagging is stored in bit 0 of BTag variable
    relationToCTag.Jet(jet);
    relationToCTag.Tag(cTag);

    // Tau-tag info
    auto tauTag           = colTauTags->create();
    auto relationToTauTag = ascColJetsToTauTags->create();
    tauTag.Value(cand->TauTag);
    relationToTauTag.Jet(jet);
    relationToTauTag.Tag(tauTag);

    // Flavour-tag info

    // Debug: print FCC-EDM jets info
    if (msgLevel() <= MSG::DEBUG) {

      double energy = sqrt(jet.Core().P4.Px*jet.Core().P4.Px +
                           jet.Core().P4.Py*jet.Core().P4.Py +
                           jet.Core().P4.Pz*jet.Core().P4.Pz +
                           jet.Core().P4.Mass*jet.Core().P4.Mass);

      debug() << "Jet: "
              << " Id: "       << std::setw(3)  << j+1
              << " Flavor: "   << std::setw(3)  << relationToFlavor.Tag().Value()
              << " BTag: "     << std::setprecision(1) << std::setw(3) << relationToBTag.Tag().Value()
              << " CTag: "     << std::setprecision(1) << std::setw(3) << relationToCTag.Tag().Value()
              << " TauTag: "   << std::setprecision(1) << std::setw(3) << relationToTauTag.Tag().Value()
              << std::scientific
              << " Px: "       << std::setprecision(2) << std::setw(9) << jet.Core().P4.Px
              << " Py: "       << std::setprecision(2) << std::setw(9) << jet.Core().P4.Py
              << " Pz: "       << std::setprecision(2) << std::setw(9) << jet.Core().P4.Pz
              << " E: "        << std::setprecision(2) << std::setw(9) << energy
              << " M: "        << std::setprecision(2) << std::setw(9) << jet.Core().P4.Mass
              << std::fixed
              << std::endl;
    }

    // Get corresponding jet constituents
    for (auto itCand=cand->GetCandidates()->begin(); itCand!=cand->GetCandidates()->end(); ++itCand) {

      // Jet constituent
      Candidate* candJetPart = static_cast<Candidate*>(*itCand);

      auto itRefID = refIDDelphIDFCC.find(candJetPart->GetUniqueID());

      // Jet part already saved in collection --> make only relation
      if (itRefID!=refIDDelphIDFCC.end()) {

        auto index = itRefID->second;

        if (index>=0) {
          auto jetPart        = colJetParts->at(index);
          auto relationToPart = ascColJetsToPart->create();
          relationToPart.Jet(jet);
          relationToPart.Particle(jetPart);

          debug() << "  Constituent Old - idxid: "  << candJetPart->GetUniqueID() << " " << jetPart.getObjectID().index
                  << std::setprecision(2)
                  << std::scientific
                  << " Px: " << std::setw(9) << jetPart.Core().P4.Px
                  << " Py: " << std::setw(9) << jetPart.Core().P4.Py
                  << " Pz: " << std::setw(9) << jetPart.Core().P4.Pz
                  << " M: "  << std::setw(9) << jetPart.Core().P4.Mass
                  << std::fixed
                  << std::endl;
        }
        else {
          error() << "Algorithm assigning jet constituents failed - check" << endmsg;
        }
      }
      // Jet part not found --> create it + make relation
      else {

        auto jetPart      = colJetParts->create();
        auto barePart     = fcc::BareParticle();
        barePart.Type     = 0;
        barePart.Status   = 0;
        barePart.P4.Px    = candJetPart->Momentum.Px();
        barePart.P4.Py    = candJetPart->Momentum.Py();
        barePart.P4.Pz    = candJetPart->Momentum.Pz();
        barePart.P4.Mass  = candJetPart->Momentum.M();
        barePart.Charge   = candJetPart->Charge;
        barePart.Vertex.X = 0;
        barePart.Vertex.Y = 0;
        barePart.Vertex.Z = 0;
        jetPart.Core(barePart);

        auto relationToPart = ascColJetsToPart->create();
        relationToPart.Jet(jet);
        relationToPart.Particle(jetPart);

        // Assign fcc constituent id to delphes constituent id
        refIDDelphIDFCC[candJetPart->GetUniqueID()] = jetPart.getObjectID().index;

        debug() << "  Constituent New - idxid: "  << candJetPart->GetUniqueID() << " " << jetPart.getObjectID().index
                << std::setprecision(2)
                << std::scientific
                << " Px: " << std::setw(9) << jetPart.Core().P4.Px
                << " Py: " << std::setw(9) << jetPart.Core().P4.Py
                << " Pz: " << std::setw(9) << jetPart.Core().P4.Pz
                << " M: "  << std::setw(9) << jetPart.Core().P4.Mass
                << std::fixed
                << std::endl;
      }

    } // Jet constituents

    // Debug: print end-line
    if (msgLevel() <= MSG::DEBUG) debug() << endmsg;

  } // For - jets
  return StatusCode::SUCCESS;
}
