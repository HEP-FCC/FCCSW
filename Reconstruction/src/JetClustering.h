#ifndef _JETCLUSTERING_H_
#define _JETCLUSTERING_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DataObjects/ParticleCollection.h"
#include "DataObjects/JetCollection.h"
#include "GaudiKernel/DataObjectHandle.h"

#include "fastjet/JetDefinition.hh"

class JetClustering: public GaudiAlgorithm {
  friend class AlgFactory<JetClustering> ;

public:

  JetClustering(const std::string& name, ISvcLocator* svcLoc);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  /// Handle for the HepMC to be read
  DataObjectHandle<ParticleCollection> m_genphandle;

  /// Handle for PseudoJets to be produced
  DataObjectHandle<JetCollection> m_jets;

  /// Name for the jet algorithm to be used 
  std::string m_jetAlgorithm; 
  fastjet::JetAlgorithm m_fj_jetAlgorithm;

  /// Cone radius. COLIN: not sure how it's interpreted 
  /// depending on the algorithm... should be described here
  float m_R; 

  /// Recombination scheme name
  std::string m_recombinationScheme;

  /// Recombination scheme object
  fastjet::RecombinationScheme m_fj_recombinationScheme;

  /// If true, reconstruct an arbitrary number of jets.
  /// if not, for the reconstruction of m_njets jets.
  bool m_inclusiveJets; ///< use inclusive or exclusive jets

  /// pT threshold for inclusive jets 
  float m_ptMin; 

  /// distance threshold for exclusive jets 
  float m_dcut;

  /// number of jets for exclusive jets 
  int m_njets; 

  /// verbosity flag
  bool m_verbose; 
};

#endif
