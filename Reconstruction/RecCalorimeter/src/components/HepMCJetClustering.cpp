#include "HepMCJetClustering.h"

#include "datamodel/JetCollection.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/PseudoJet.hh"

#include <vector>

using namespace fastjet;

DECLARE_COMPONENT(HepMCJetClustering)

HepMCJetClustering::HepMCJetClustering(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hepmc", m_hepmchandle, "Handle for the HepMC to be read");
  declareProperty("jets", m_jets, "Handle for PseudoJets to be produced");
}

StatusCode HepMCJetClustering::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;

  // initialize jet algorithm
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

  // initialize recombination scheme
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

  // check sanity of configuration
  if (m_inclusiveJets) {
    if (m_ptMin < 0.0) {
      error() << "Sub-zero min pT for jets" << endmsg;
      return StatusCode::FAILURE;
    }
    if (m_dcut != -1 || m_njets != -1) warning() << "Set options for exclusive jets, but inclusive jets used" << endmsg;
  } else {  // exclusive jets
    if (m_dcut == -1 && m_njets == -1) {
      error() << "Neither Dcut nor NJets configured for exclusive jets" << endmsg;
      return StatusCode::FAILURE;
    }
    if (m_ptMin != 0.0) warning() << "Set options for inclusive jets, but exclusive jets used" << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode HepMCJetClustering::execute() {
  auto theEvent = m_hepmchandle.get();

  // setup input for fastjet
  std::vector<fastjet::PseudoJet> input;
  for (auto it = theEvent->particles_begin(); it != theEvent->particles_end(); ++it) {
    auto p = *it;
    // TODO apply some filtering if required
    fastjet::PseudoJet pj(p->momentum().px(), p->momentum().py(), p->momentum().pz(), p->momentum().e());
    input.emplace_back(pj);
  }

  JetDefinition def(m_fj_jetAlgorithm, m_R, m_fj_recombinationScheme);

  ClusterSequence cs(input, def);

  auto sortedOutput =
      sorted_by_pt(m_inclusiveJets ? cs.inclusive_jets(m_ptMin)
                                   // use exclusive jets
                                   : m_njets != -1 ? cs.exclusive_jets(m_njets) : cs.exclusive_jets(m_dcut));
  auto outJets = m_jets.createAndPut();
  for (auto& jet : sortedOutput) {
    auto outJet = outJets->create();
    double px = jet.px();
    double py = jet.py();
    double pz = jet.pz();
    double e = jet.e();
    auto& p4 = outJet.p4();
    p4.px = px;
    p4.py = py;
    p4.pz = pz;
    p4.mass = std::sqrt(-px * px - py * py - pz * pz + e * e);
  }
  return StatusCode::SUCCESS;
}

StatusCode HepMCJetClustering::finalize() { return GaudiAlgorithm::finalize(); }
