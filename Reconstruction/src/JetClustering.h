#ifndef _JETCLUSTERING_H_
#define _JETCLUSTERING_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/DataObjectHandle.h"

#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"

#include "DataObjects/BareParticle.h"
#include "DataObjects/LorentzVector.h"
#include "DataObjects/BareJet.h"

#include "TLorentzVector.h"

#include <iostream>

template<class P, class J>
class JetClustering: public GaudiAlgorithm {
  // Colin: the following doesn't seem to be necessary
  // friend class AlgFactory< JetClustering<P, J> > ;

public:

  JetClustering(const std::string& name, ISvcLocator* svcLoc);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  /// Handle for the HepMC to be read
  DataObjectHandle<P> m_genphandle;

  /// Handle for PseudoJets to be produced
  DataObjectHandle<J> m_jets;

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


template<class P, class J >
  JetClustering<P, J>::JetClustering(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc)
  , m_fj_jetAlgorithm(fastjet::JetAlgorithm::undefined_jet_algorithm)
  , m_fj_recombinationScheme(fastjet::RecombinationScheme::E_scheme) {
  declareInput("particles", m_genphandle);
  declareOutput("jets", m_jets);

  declareProperty("jetAlgorithm", m_jetAlgorithm = "antikt", "the Jet Algorithm to use [kt, antikt, cambridge]");
  declareProperty("coneRadius", m_R = 0.5, "cone radius");
  declareProperty("recominbationScheme", m_recombinationScheme = "E", "the Recombination Scheme to use [E, pt, et]");
  declareProperty("inclusiveJets", m_inclusiveJets = true, "use inclusive or exclusive jets");
  declareProperty("ptMin", m_ptMin = 10, "Minimum pT of jets for inclusiveJets");
  declareProperty("dcut", m_dcut = -1, "dcut for exclusive jets");
  declareProperty("nJets", m_njets = -1.0, "Number of jets for exclusive jets");
  declareProperty("verbose", m_verbose = false, "Boolean flag for verbosity");
}

template<class P, class J>
  StatusCode JetClustering<P, J>::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;

  //initialize jet algorithm
  if (m_jetAlgorithm == "kt")
    m_fj_jetAlgorithm = fastjet::JetAlgorithm::kt_algorithm;
  else if (m_jetAlgorithm == "antikt")
    m_fj_jetAlgorithm = fastjet::JetAlgorithm::antikt_algorithm;
  else if (m_jetAlgorithm == "cambridge")
    m_fj_jetAlgorithm = fastjet::JetAlgorithm::cambridge_algorithm;
  else {
    error() << "Undefined jet algorithm specified" << endmsg;
    return StatusCode::FAILURE;
  }

  //initialize recombination scheme
  if (m_recombinationScheme == "E") {
    m_fj_recombinationScheme = fastjet::RecombinationScheme::E_scheme;
  } else if (m_recombinationScheme == "pt") {
    m_fj_recombinationScheme = fastjet::RecombinationScheme::pt_scheme;
  } else if (m_recombinationScheme == "et") {
    m_fj_recombinationScheme = fastjet::RecombinationScheme::Et_scheme;
  } else {
    error() << "Undefined recombination scheme specified" << endmsg;
    return StatusCode::FAILURE;
  }

  //check sanity of configuration
  if (m_inclusiveJets) {
    if (m_ptMin < 0.0) {
      error() << "Sub-zero min pT for jets" << endmsg;
      return StatusCode::FAILURE;
    }
    if (m_dcut != -1 || m_njets != -1)
      warning()
	<< "Set options for exclusive jets, but inclusive jets used"
	<< endmsg;
  } else { //exclusive jets
    if (m_dcut == -1 && m_njets == -1) {
      error() << "Neither Dcut nor NJets configured for exclusive jets"
	      << endmsg;
      return StatusCode::FAILURE;
    }
    if (m_ptMin != 0.0)
      warning()
	<< "Set options for inclusive jets, but exclusive jets used"
	<< endmsg;
  }

  return StatusCode::SUCCESS;
}

template< class P, class J>
  StatusCode JetClustering<P, J>::execute() {

  //setup input for fastjet
  const P* particles = m_genphandle.get();
  std::vector<fastjet::PseudoJet> input;
  for (auto it = particles->begin(); it != particles->end(); ++it) {
    auto ptchandle = *it;
    const BareParticle& ptc = ptchandle.read().Core;
    TLorentzVector p4; 
    p4.SetPtEtaPhiM(ptc.P4.Pt, ptc.P4.Eta, 
                    ptc.P4.Phi, ptc.P4.Mass);
    //TODO apply some filtering if required
    fastjet::PseudoJet pj(p4.Px(), p4.Py(), p4.Pz(), p4.E());
    input.emplace_back(pj);
  }
  
  fastjet::JetDefinition def(m_fj_jetAlgorithm, m_R, m_fj_recombinationScheme);
  fastjet::ClusterSequence cs(input, def);
  // fastjet::PseudoJetEntry * output = new fastjet::PseudoJetEntry();
  // output->setJets(sorted_by_pt( m_inclusiveJets ? cs.inclusive_jets(m_ptMin)
  //use exclusive jets
  //                            : m_njets != -1 ? cs.exclusive_jets(m_njets) : cs.exclusive_jets(m_dcut) ));
  //  std::vector<fastjet::PseudoJet> pjets = m_inclusiveJets ? cs.inclusive_jets(m_ptMin)
  //  : m_njets != -1 ? cs.exclusive_jets(m_njets) : cs.exclusive_jets(m_dcut);
  std::vector<fastjet::PseudoJet> pjets = cs.inclusive_jets(m_ptMin);

  J* jets = new J();
  if(m_verbose)
    std::cout<<"njets = "<<pjets.size()<<std::endl;
  for(auto pjet : pjets) {
    auto& jet = jets->create();
    BareJet& core = jet.mod().Core; 
    core.P4.Pt = pjet.pt();
    core.P4.Eta = pjet.eta();
    core.P4.Phi = pjet.phi();
    core.P4.Mass = pjet.m();
    //COLIN need to set the jet area
    if(m_verbose)
      std::cout<<pjet.e()<<" "<<pjet.eta()<<" "<<pjet.phi()<<std::endl;
  }
  m_jets.put(jets);
  // m_jets.put(output);
  
  return StatusCode::SUCCESS;
}

template<class P, class J>
  StatusCode JetClustering<P, J>::finalize() {
  return GaudiAlgorithm::finalize();
}


#endif
