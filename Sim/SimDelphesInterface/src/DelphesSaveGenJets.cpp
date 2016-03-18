#include "DelphesSaveGenJets.h"

// Delphes
#include "modules/Delphes.h"
#include "classes/DelphesClasses.h"

// datamodel
#include "datamodel/GenJetCollection.h"
#include "datamodel/GenJetParticleAssociationCollection.h"
#include "datamodel/IntTagCollection.h"
#include "datamodel/GenJetIntTagAssociationCollection.h"
#include "datamodel/MCParticleCollection.h"

// ROOT
#include "TObjArray.h"

DECLARE_TOOL_FACTORY(DelphesSaveGenJets)

DelphesSaveGenJets::DelphesSaveGenJets(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  GaudiTool(aType, aName, aParent) {
  declareInterface<IDelphesSaveOutputTool>(this);
  declareOutput("genJets", m_genJets);
  declareOutput("mcAssociations", m_mcAssociations);
  declareOutput("jetFlavorTags", m_jetFlavorTags);
  declareOutput("jetFlavorAssociations", m_jetFlavorAssociations);
  declareProperty("delphesArrayName", m_delphesArrayName);
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

DelphesSaveGenJets::~DelphesSaveGenJets() {}

StatusCode DelphesSaveGenJets::initialize() {
  return GaudiTool::initialize();
}

StatusCode DelphesSaveGenJets::finalize() {
  return GaudiTool::finalize();
}

StatusCode DelphesSaveGenJets::saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) {
  // Create the collections
  auto colGenJets = m_genJets.createAndPut();
  auto colJetsFlavor = m_jetFlavorTags.createAndPut();
  auto ascColJetsToFlavor = m_jetFlavorAssociations.createAndPut();
  auto ascColGenJetsToMC = m_mcAssociations.createAndPut();

  const TObjArray* delphesColl = delphes.ImportArray(m_delphesArrayName.c_str());
  if (delphesColl == nullptr) {
    warning() << "Delphes collection " << m_delphesArrayName << " not present. Skipping it." << endmsg;
    return StatusCode::SUCCESS;
  }

  for(int j = 0; j < delphesColl->GetEntries(); ++j) {

    auto cand = static_cast<Candidate *>(delphesColl->At(j));

    // Jet info
    auto jet         = colGenJets->create();
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

    // Debug: print FCC-EDM jets info
    if (msgLevel() <= MSG::DEBUG) {

      double energy = sqrt(jet.Core().P4.Px*jet.Core().P4.Px +
                           jet.Core().P4.Py*jet.Core().P4.Py +
                           jet.Core().P4.Pz*jet.Core().P4.Pz +
                           jet.Core().P4.Mass*jet.Core().P4.Mass);

      debug() << "Gen Jet: "
              << " Id: "       << std::setw(3)  << j+1
              << " Flavor: "   << std::setw(3)  << relationToFlavor.Tag().Value()
              << std::scientific
              << " Px: "       << std::setprecision(2) << std::setw(9) << jet.Core().P4.Px
              << " Py: "       << std::setprecision(2) << std::setw(9) << jet.Core().P4.Py
              << " Pz: "       << std::setprecision(2) << std::setw(9) << jet.Core().P4.Pz
              << " E: "        << std::setprecision(2) << std::setw(9) << energy
              << " M: "        << std::setprecision(2) << std::setw(9) << jet.Core().P4.Mass
              << std::fixed
              << std::endl;
    }

    // Reference to MC - Delphes holds references to all objects related to the Jet object,
    // several relations might exist -> find "recursively" in a tree history the MC particle.
    // Add index to the reference index field to avoid double counting
    std::set<int> idRefMCPart; // Avoid double counting when referencingh MC particles

    // Recursive procedure stops after the relation found is the one to MC particle and not to
    // a particle object. If particle not related to MC particle (<0 value)
    findJetPartMC(cand, mcParticles.size(), idRefMCPart);

    // Debug: print variable
    double totSimE = 0;

    for (auto id : idRefMCPart) {

      auto relationToMC = ascColGenJetsToMC->create();
      relationToMC.Jet(jet);
      relationToMC.Particle(mcParticles.at(id));

      // Debug: print FCC-EDM jet relation info
      if (msgLevel() <= MSG::DEBUG) {
        double recE   = sqrt(relationToMC.Jet().Core().P4.Px*relationToMC.Jet().Core().P4.Px +
                             relationToMC.Jet().Core().P4.Py*relationToMC.Jet().Core().P4.Py +
                             relationToMC.Jet().Core().P4.Pz*relationToMC.Jet().Core().P4.Pz +
                             relationToMC.Jet().Core().P4.Mass*relationToMC.Jet().Core().P4.Mass);
        double simE   = sqrt(relationToMC.Particle().Core().P4.Px*relationToMC.Particle().Core().P4.Px +
                             relationToMC.Particle().Core().P4.Py*relationToMC.Particle().Core().P4.Py +
                             relationToMC.Particle().Core().P4.Pz*relationToMC.Particle().Core().P4.Pz +
                             relationToMC.Particle().Core().P4.Mass*relationToMC.Particle().Core().P4.Mass);
        totSimE += simE;
        debug() << " RefId: " << std::setw(3)            << id+1
                << " Rel E: " << std::setprecision(2)
                              << std::scientific
                              << std::setw(9) << simE    << " "
                              << std::setw(9) << totSimE << " <-> "
                              << std::setw(9) << recE
                              << std::fixed
                              << std::endl;
      } // Debug
    }

    // Debug: print end-line
    if (msgLevel() <= MSG::DEBUG) debug() << endmsg;
  } // For - jets
  return StatusCode::SUCCESS;
}

void DelphesSaveGenJets::findJetPartMC(Candidate* cand, int rangeMCPart, std::set<int>& idRefMCPart) {
  // Warning - no MC relation found
  if (cand->GetCandidates()->GetEntries()==0) {

    warning() << "Can't build one of the relations from Jet to MC particle!" << std::endl;
  }
  // Relation can be found
  else {

    for (auto itCand=cand->GetCandidates()->begin(); itCand!=cand->GetCandidates()->end(); ++itCand) {

      Candidate* refCand = static_cast<Candidate*>(*itCand);
      int id = refCand->GetUniqueID()-1;

      //std::cout << "Depth: " << depth << " " << id << std::endl;
      // Relation found
      if (id<rangeMCPart) {
        //std::cout << ">>> " << id << std::endl;
        idRefMCPart.insert(id);
      }
      // Not found -> step one level below
      else findJetPartMC(refCand, rangeMCPart, idRefMCPart);
    }
  }
}
