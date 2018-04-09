#ifndef RECO_JETCLUSTERING_H
#define RECO_JETCLUSTERING_H

#include "GaudiAlg/GaudiAlgorithm.h"

#include "FWCore/DataHandle.h"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/JetDefinition.hh"

#include "datamodel/BareJet.h"
#include "datamodel/BareParticle.h"
#include "datamodel/LorentzVector.h"

#include "TLorentzVector.h"

#include <iostream>

template <class P, class J>
class JetClustering : public GaudiAlgorithm {
  // Colin: the following doesn't seem to be necessary
  // friend class AlgFactory< JetClustering<P, J> > ;

public:
  JetClustering(const std::string& name, ISvcLocator* svcLoc);
  virtual ~JetClustering() {}
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  /// Handle for the particles to be read
  DataHandle<P> m_genphandle{"Particles", Gaudi::DataHandle::Reader, this};

  /// Handle for PseudoJets to be produced
  DataHandle<J> m_jets{"Jets", Gaudi::DataHandle::Writer, this};
  /// Name for the jet algorithm to be used
  Gaudi::Property<std::string> m_jetAlgorithm{this, "jetAlgorithm", "antikt",
                                              "he Jet Algorithm to use [kt, antikt, cambridge]"};
  fastjet::JetAlgorithm m_fj_jetAlgorithm{fastjet::JetAlgorithm::undefined_jet_algorithm};

  /// Cone radius. COLIN: not sure how it's interpreted
  /// depending on the algorithm... should be described here
  Gaudi::Property<float> m_R{this, "coneRadius", 0.5, "cone radius"};

  /// Recombination scheme name
  Gaudi::Property<std::string> m_recombinationScheme{this, "recombinationScheme", "E",
                                                     "the Recombination Scheme to use [E, pt, et]"};

  /// Recombination scheme object
  fastjet::RecombinationScheme m_fj_recombinationScheme{fastjet::RecombinationScheme::E_scheme};

  /// If true, reconstruct an arbitrary number of jets.
  /// if not, for the reconstruction of m_njets jets.
  Gaudi::Property<bool> m_inclusiveJets{this, "inclusiveJets", true, "use inclusive or exclusive jets"};

  /// pT threshold for inclusive jets
  Gaudi::Property<float> m_ptMin{this, "ptMin", 10, "Minimum pT of jets for inclusiveJets"};

  /// distance threshold for exclusive jets
  Gaudi::Property<float> m_dcut{this, "dcut", -1, "dcut for exclusive jets"};

  /// number of jets for exclusive jets
  Gaudi::Property<int> m_njets{this, "nJets", -1, "Number of jets for exclusive jets"};

  /// name of the area calculation method
  Gaudi::Property<std::string> m_areaTypeName{this, "areaType", "none", "Area type [none, active, passive]"};

  /// type of area calculation
  fastjet::AreaType m_areaType{fastjet::invalid_area};
};

template <class P, class J>
JetClustering<P, J>::JetClustering(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("particles", m_genphandle, "Handle for the input particles");
  declareProperty("jets", m_jets, "Handle for PseudoJets to be produced");
}

template <class P, class J>
StatusCode JetClustering<P, J>::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;

  // initialize jet algorithm
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

  // initialize recombination scheme
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

  // jet area
  if (m_areaTypeName == "active") {
    m_areaType = fastjet::active_area;
  } else if (m_areaTypeName == "passive") {
    m_areaType = fastjet::passive_area;
  } else {
    m_areaType = fastjet::invalid_area;
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

template <class P, class J>
StatusCode JetClustering<P, J>::execute() {

  // setup input for fastjet
  const P* particles = m_genphandle.get();
  std::vector<fastjet::PseudoJet> input;
  unsigned index = 0;
  for (auto it = particles->begin(); it != particles->end(); ++it) {
    auto ptchandle = *it;
    auto& ptc = ptchandle.core();
    TLorentzVector p4;
    p4.SetXYZM(ptc.p4.px, ptc.p4.py, ptc.p4.pz, ptc.p4.mass);
    // TODO apply some filtering if required
    input.emplace_back(p4.Px(), p4.Py(), p4.Pz(), p4.E());
    input.back().set_user_index(index);
    ++index;
  }

  fastjet::ClusterSequence* cs;

  fastjet::JetDefinition def(m_fj_jetAlgorithm, m_R, m_fj_recombinationScheme);
  if (m_areaType != fastjet::invalid_area) {
    fastjet::AreaDefinition areadef(m_areaType);
    std::cout << m_areaType << std::endl;
    cs = new fastjet::ClusterSequenceArea(input, def, areadef);
  } else {
    cs = new fastjet::ClusterSequence(input, def);
  }
  // fastjet::PseudoJetEntry * output = new fastjet::PseudoJetEntry();
  // output->setJets(sorted_by_pt( m_inclusiveJets ? cs.inclusive_jets(m_ptMin)
  // use exclusive jets
  //                            : m_njets != -1 ? cs.exclusive_jets(m_njets) : cs.exclusive_jets(m_dcut) ));
  //  std::vector<fastjet::PseudoJet> pjets = m_inclusiveJets ? cs.inclusive_jets(m_ptMin)
  //  : m_njets != -1 ? cs.exclusive_jets(m_njets) : cs.exclusive_jets(m_dcut);
  std::vector<fastjet::PseudoJet> pjets = fastjet::sorted_by_pt(cs->inclusive_jets(m_ptMin));

  J* jets = new J();
  debug() << "njets = " << pjets.size() << endmsg;
  for (const auto& pjet : pjets) {
    debug() << pjet.e() << " " << pjet.pt() << " " << pjet.eta() << " " << pjet.phi() << endmsg;
    auto jet = jets->create();
    auto& core = jet.core();
    core.p4.px = pjet.px();
    core.p4.py = pjet.py();
    core.p4.pz = pjet.pz();
    core.p4.mass = pjet.m();
    if (pjet.has_area())
      core.area = pjet.area();
    else
      core.area = -1;
    const std::vector<fastjet::PseudoJet>& constituents = pjet.constituents();
    for (const auto& constit : constituents) {
      debug() << "\t" << constit.user_index() << endmsg;
      jet.addparticles(particles->at(constit.user_index()));
    }
  }
  m_jets.put(jets);

  delete cs;
  return StatusCode::SUCCESS;
}

template <class P, class J>
StatusCode JetClustering<P, J>::finalize() {
  return GaudiAlgorithm::finalize();
}

#endif
