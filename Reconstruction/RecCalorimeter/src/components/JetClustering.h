#ifndef RECO_JETCLUSTERING_H
#define RECO_JETCLUSTERING_H

#include "GaudiAlg/GaudiAlgorithm.h"

#include "FWCore/DataHandle.h"
#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"

#include "datamodel/BareParticle.h"
#include "datamodel/LorentzVector.h"
#include "datamodel/BareJet.h"

#include "TLorentzVector.h"

#include <iostream>

template<class P, class J>
class JetClustering: public GaudiAlgorithm {
  // Colin: the following doesn't seem to be necessary
  // friend class AlgFactory< JetClustering<P, J> > ;

public:

  JetClustering(const std::string& name, ISvcLocator* svcLoc);
  virtual ~JetClustering() {}
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  /// Handle for the HepMC to be read
  DataHandle<P> m_genphandle;

  /// Handle for PseudoJets to be produced
  DataHandle<J> m_jets;

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

  /// name of the area calculation method
  std::string m_areaTypeName;

  /// type of area calculation
  fastjet::AreaType m_areaType;

  /// verbosity flag
  bool m_verbose;
};


template<class P, class J>
  JetClustering<P, J>::JetClustering(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc)
  , m_fj_jetAlgorithm(fastjet::JetAlgorithm::undefined_jet_algorithm)
  , m_fj_recombinationScheme(fastjet::RecombinationScheme::E_scheme)
  , m_areaType(fastjet::invalid_area)
{
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
  declareProperty("areaType", m_areaTypeName = "none", "Area type [none, active, passive]");
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

  //jet area
  if (m_areaTypeName == "active") {
    m_areaType = fastjet::active_area;
  }
  else if (m_areaTypeName == "passive") {
    m_areaType = fastjet::passive_area;
  }
  else {
    m_areaType = fastjet::invalid_area;
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
  unsigned index = 0;
  for (auto it = particles->begin(); it != particles->end(); ++it) {
    auto ptchandle = *it;
    auto& ptc = ptchandle.core();
    TLorentzVector p4;
    p4.SetXYZM(ptc.p4.px, ptc.p4.py,
                    ptc.p4.pz, ptc.p4.mass);
    //TODO apply some filtering if required
    input.emplace_back(p4.Px(), p4.Py(), p4.Pz(), p4.E());
    input.back().set_user_index(index);
    ++index;
  }

  fastjet::ClusterSequence* cs;

  fastjet::JetDefinition def(m_fj_jetAlgorithm, m_R, m_fj_recombinationScheme);
  if(m_areaType != fastjet::invalid_area) {
    fastjet::AreaDefinition areadef(m_areaType);
    std::cout<<m_areaType<<std::endl;
    cs = new fastjet::ClusterSequenceArea(input, def, areadef);
  }
  else {
    cs = new fastjet::ClusterSequence(input, def);
  }
  // fastjet::PseudoJetEntry * output = new fastjet::PseudoJetEntry();
  // output->setJets(sorted_by_pt( m_inclusiveJets ? cs.inclusive_jets(m_ptMin)
  //use exclusive jets
  //                            : m_njets != -1 ? cs.exclusive_jets(m_njets) : cs.exclusive_jets(m_dcut) ));
  //  std::vector<fastjet::PseudoJet> pjets = m_inclusiveJets ? cs.inclusive_jets(m_ptMin)
  //  : m_njets != -1 ? cs.exclusive_jets(m_njets) : cs.exclusive_jets(m_dcut);
  std::vector<fastjet::PseudoJet> pjets = fastjet::sorted_by_pt(cs->inclusive_jets(m_ptMin));

  J* jets = new J();
  if(m_verbose)
    std::cout<<"njets = "<<pjets.size()<<std::endl;
  for(const auto& pjet : pjets) {
    if(m_verbose)
      std::cout<<pjet.e()<<" "<<pjet.pt()<<" "<<pjet.eta()<<" "<<pjet.phi()<<std::endl;
    auto jet = jets->create();
    auto& core = jet.core();
    core.p4.px = pjet.px();
    core.p4.py = pjet.py();
    core.p4.pz = pjet.pz();
    core.p4.mass = pjet.m();
    if(pjet.has_area())
      core.area = pjet.area();
    else
      core.area = -1;
    const std::vector<fastjet::PseudoJet>& constituents = pjet.constituents();
    for(const auto& constit : constituents) {
      if(m_verbose)
        std::cout<<"\t"<<constit.user_index()<<std::endl;
      jet.addparticles(particles->at(constit.user_index()));
    }
  }
  m_jets.put(jets);

  delete cs;
  return StatusCode::SUCCESS;
}

template<class P, class J>
  StatusCode JetClustering<P, J>::finalize() {
  return GaudiAlgorithm::finalize();
}


#endif
