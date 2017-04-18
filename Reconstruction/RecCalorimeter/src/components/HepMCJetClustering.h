#ifndef RECO_HEPMCJETCLUSTERING_H
#define RECO_HEPMCJETCLUSTERING_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HepMC/GenEvent.h"
#include "datamodel/JetCollection.h"

#include "fastjet/JetDefinition.hh"

class HepMCJetClustering : public GaudiAlgorithm {
  friend class AlgFactory<HepMCJetClustering>;

public:
  /// Constructor.
  HepMCJetClustering(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the HepMC to be read
  DataHandle<HepMC::GenEvent> m_hepmchandle{"HepMC", Gaudi::DataHandle::Reader, this};

  /// Handle for PseudoJets to be produced
  DataHandle<fcc::JetCollection> m_jets{"GenJets", Gaudi::DataHandle::Writer, this};

  /// the jet algorithm to use
  Gaudi::Property<std::string> m_jetAlgorithm{this, "JetAlgorithm", "kt",
                                              "the Jet Algorithm to use [kt, antikt, cambridge]"};
  fastjet::JetAlgorithm m_fj_jetAlgorithm{fastjet::JetAlgorithm::undefined_jet_algorithm};

  /// the cone radius
  Gaudi::Property<float> m_R{this, "ConeRadius", 1.0, "cone radius"};
  /// the recombination scheme
  Gaudi::Property<std::string> m_recombinationScheme{this, "RecominbationScheme", "E",
                                                     "the Recombination Scheme to use [E, pt, et]"};
  fastjet::RecombinationScheme m_fj_recombinationScheme{fastjet::RecombinationScheme::E_scheme};

  /// use inclusive or exclusive jets
  Gaudi::Property<bool> m_inclusiveJets{this, "InclusiveJets", true, "use inclusive or exclusive jets"};
  /// minimum pT for inclusiveJets
  Gaudi::Property<float> m_ptMin{this, "PtMin", 0.0, "Minimum pT of jets for inclusiveJets"};
  /// dCut for exclusiveJets
  Gaudi::Property<float> m_dcut{this, "Dcut", -1, "dcut for exclusive jets"};
  /// number of jets for exclusiveJets
  Gaudi::Property<int> m_njets{this, "NJets", -1, "Number of jets for exclusive jets"};
};

#endif
