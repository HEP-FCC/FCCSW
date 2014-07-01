#include "HepMCJetClustering.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"

#include <vector>

using namespace fastjet;

DECLARE_COMPONENT(HepMCJetClustering)

HepMCJetClustering::HepMCJetClustering(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc)
  , m_fj_jetAlgorithm(JetAlgorithm::undefined_jet_algorithm)
  , m_fj_recombinationScheme(RecombinationScheme::E_scheme)
{
  declareInput("hepmc", m_hepmchandle);
  declareOutput("jets", m_jets);

  declareProperty("JetAlgorithm", m_jetAlgorithm = "kt", "the Jet Algorithm to use [kt, antikt, cambridge]");
  declareProperty("ConeRadius", m_R = 1.0, "cone radius");
  declareProperty("RecominbationScheme", m_recombinationScheme = "E", "the Recombination Scheme to use [E, pt, et]");
  declareProperty("InclusiveJets", m_inclusiveJets = true, "use inclusive or exclusive jets");
  declareProperty("PtMin", m_ptMin = 0.0, "Minimum pT of jets for inclusiveJets");
  declareProperty("Dcut", m_dcut = -1, "dcut for exclusive jets");
  declareProperty("NJets", m_njets = -1.0, "Number of jets for exclusive jets");
}

StatusCode HepMCJetClustering::initialize() {
  if(GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;

  //initialize jet algorithm
  if(m_jetAlgorithm == "kt")
      m_fj_jetAlgorithm = JetAlgorithm::kt_algorithm;
  else if(m_jetAlgorithm == "antikt") 
      m_fj_jetAlgorithm = JetAlgorithm::antikt_algorithm;
  else if(m_jetAlgorithm == "cambridge")
      m_fj_jetAlgorithm = JetAlgorithm::cambridge_algorithm;
  if(m_fj_jetAlgorithm == JetAlgorithm::undefined_jet_algorithm){
    error() << "Undefined jet algorithm specified" << endmsg;
    return StatusCode::FAILURE;
  }

  //initialize recombination scheme
  if(m_recombinationScheme == "E"){
   m_fj_recombinationScheme = RecombinationScheme::E_scheme;
  } else if (m_recombinationScheme == "pt"){
     m_fj_recombinationScheme = RecombinationScheme::pt_scheme;
  } else if (m_recombinationScheme == "et"){
     m_fj_recombinationScheme = RecombinationScheme::Et_scheme;
  } else {
     error() << "Undefined recombination scheme specified" << endmsg;
     return StatusCode::FAILURE;
  }

  //check sanity of configuration
  if(m_inclusiveJets){
    if(m_ptMin < 0.0){
      error() << "Sub-zero min pT for jets" << endmsg;
      return StatusCode::FAILURE;
    }
    if(m_dcut != -1 || m_njets != -1)
      warning() << "Set options for exclusive jets, but inclusive jets used" << endmsg;
    } else { //exclusive jets
      if(m_dcut == -1 && m_njets == -1){
	error() << "Neither Dcut nor NJets configured for exclusive jets" << endmsg;
      return StatusCode::FAILURE;
    }
    if(m_ptMin != 0.0)
      warning() << "Set options for inclusive jets, but exclusive jets used" << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode HepMCJetClustering::execute() {
        auto theEvent = m_hepmchandle.get()->getEvent();

    //setup input for fastjet
    std::vector<PseudoJet> input;
    for (auto it = theEvent->particles_begin(); it != theEvent->particles_end(); ++it) {
         auto p = *it;
         //TODO apply some filtering if required
         fastjet::PseudoJet pj(p->momentum().px(),
                               p->momentum().py(),
                               p->momentum().pz(),
                               p->momentum().e()  );
         input.emplace_back(pj);
    }

    JetDefinition def(m_fj_jetAlgorithm, m_R, m_fj_recombinationScheme);

    ClusterSequence cs(input, def);

    PseudoJetEntry * output = new PseudoJetEntry();
    output->setJets(sorted_by_pt( m_inclusiveJets ? cs.inclusive_jets(m_ptMin)
                                  //use exclusive jets
                                 : m_njets != -1 ? cs.exclusive_jets(m_njets) : cs.exclusive_jets(m_dcut) ));


    m_jets.put(output);

    return StatusCode::SUCCESS;
}

StatusCode HepMCJetClustering::finalize() {
  return GaudiAlgorithm::finalize();
}
