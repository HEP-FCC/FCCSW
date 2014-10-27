#include "JetClustering.h"

#include "DataObjects/LorentzVector.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"

#include "TLorentzVector.h"

#include <vector>

using namespace fastjet;

DECLARE_COMPONENT(JetClustering)

JetClustering::JetClustering(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc)
  , m_fj_jetAlgorithm(JetAlgorithm::undefined_jet_algorithm)
  , m_fj_recombinationScheme(RecombinationScheme::E_scheme)
{
  declareInput("genparticles", m_genphandle);
  declareOutput("jets", m_jets);

  declareProperty("JetAlgorithm", m_jetAlgorithm = "antikt", "the Jet Algorithm to use [kt, antikt, cambridge]");
  declareProperty("ConeRadius", m_R = 0.5, "cone radius");
  declareProperty("RecominbationScheme", m_recombinationScheme = "E", "the Recombination Scheme to use [E, pt, et]");
  declareProperty("InclusiveJets", m_inclusiveJets = true, "use inclusive or exclusive jets");
  declareProperty("PtMin", m_ptMin = 10, "Minimum pT of jets for inclusiveJets");
  declareProperty("Dcut", m_dcut = -1, "dcut for exclusive jets");
  declareProperty("NJets", m_njets = -1.0, "Number of jets for exclusive jets");
}

StatusCode JetClustering::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;

  //initialize jet algorithm
  if (m_jetAlgorithm == "kt")
    m_fj_jetAlgorithm = JetAlgorithm::kt_algorithm;
  else if (m_jetAlgorithm == "antikt")
    m_fj_jetAlgorithm = JetAlgorithm::antikt_algorithm;
  else if (m_jetAlgorithm == "cambridge")
    m_fj_jetAlgorithm = JetAlgorithm::cambridge_algorithm;
  else {
    error() << "Undefined jet algorithm specified" << endmsg;
    return StatusCode::FAILURE;
  }

  //initialize recombination scheme
  if (m_recombinationScheme == "E") {
    m_fj_recombinationScheme = RecombinationScheme::E_scheme;
  } else if (m_recombinationScheme == "pt") {
    m_fj_recombinationScheme = RecombinationScheme::pt_scheme;
  } else if (m_recombinationScheme == "et") {
    m_fj_recombinationScheme = RecombinationScheme::Et_scheme;
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

StatusCode JetClustering::execute() {

  //setup input for fastjet
  const ParticleCollection* particles = m_genphandle.get();
  std::vector<PseudoJet> input;
  for (auto it = particles->begin(); it != particles->end(); ++it) {
    auto p = *it;
    TLorentzVector p4; 
    p4.SetPtEtaPhiM(p.P4().Pt, p.P4().Eta, p.P4().Phi, p.P4().Mass);
    //TODO apply some filtering if required
    fastjet::PseudoJet pj(p4.Px(), p4.Py(), p4.Pz(), p4.E());
        input.emplace_back(pj);
  }
  
  JetDefinition def(m_fj_jetAlgorithm, m_R, m_fj_recombinationScheme);
  ClusterSequence cs(input, def);
  // PseudoJetEntry * output = new PseudoJetEntry();
  // output->setJets(sorted_by_pt( m_inclusiveJets ? cs.inclusive_jets(m_ptMin)
  //use exclusive jets
  // 				: m_njets != -1 ? cs.exclusive_jets(m_njets) : cs.exclusive_jets(m_dcut) ));
  //  std::vector<PseudoJet> pjets = m_inclusiveJets ? cs.inclusive_jets(m_ptMin)
  //  : m_njets != -1 ? cs.exclusive_jets(m_njets) : cs.exclusive_jets(m_dcut);
  std::vector<PseudoJet> pjets = cs.inclusive_jets(m_ptMin);

  JetCollection* jets = new JetCollection();
  for(auto pjet : pjets) {
    JetHandle& jet = jets->create();
    LorentzVector p4;
    p4.Pt = pjet.pt();
    p4.Eta = pjet.eta();
    p4.Phi = pjet.phi();
    p4.Mass = pjet.m();
    jet.setP4( p4 ); 
    // std::cout<<pjet.pt()<<std::endl;
  }
  m_jets.put(jets);
  // m_jets.put(output);
  
  return StatusCode::SUCCESS;
}

StatusCode JetClustering::finalize() {
  return GaudiAlgorithm::finalize();
}
