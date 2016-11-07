#ifndef RECO_HEPMCJETCLUSTERING_H
#define RECO_HEPMCJETCLUSTERING_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "datamodel/JetCollection.h"
#include "HepMC/GenEvent.h"
#include "FWCore/DataHandle.h"

#include "fastjet/JetDefinition.hh"

class HepMCJetClustering: public GaudiAlgorithm {
  friend class AlgFactory<HepMCJetClustering> ;

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
  DataHandle<HepMC::GenEvent> m_hepmchandle;

  /// Handle for PseudoJets to be produced
  DataHandle<fcc::JetCollection> m_jets;

  std::string m_jetAlgorithm; ///< the jet algorithm to use
  fastjet::JetAlgorithm m_fj_jetAlgorithm;

  float m_R; ///< the cone radius

  std::string m_recombinationScheme; ///< the recombination scheme
  fastjet::RecombinationScheme m_fj_recombinationScheme;

  bool m_inclusiveJets; ///< use inclusive or exclusive jets
  float m_ptMin; ///< minimum pT for inclusiveJets
  float m_dcut; ///< dCut for exclusiveJets
  int m_njets; ///< number of jets for exclusiveJets
};

#endif
