#include "PythiaInterface.h"

// Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"

// Pythia
#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

// Include UserHooks for Jet Matching.
#include "Pythia8Plugins/CombineMatchingInput.h"
#include "Pythia8Plugins/JetMatching.h"
// Include UserHooks for randomly choosing between integrated and
// non-integrated treatment for unitarised merging.
#include "Pythia8Plugins/aMCatNLOHooks.h"

// FCCSW
#include "Generation/Units.h"

// FCC EDM
#include "datamodel/FloatCollection.h"

DECLARE_COMPONENT(PythiaInterface)

PythiaInterface::PythiaInterface(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc), m_pythiaSignal(nullptr), m_parfile(), m_nAbort(0), m_iAbort(0), m_iEvent(0),
                 m_doMePsMatching(0), m_doMePsMerging(0),
                 m_matching(nullptr), m_setting(nullptr) {

  declareProperty("Filename", m_parfile="", "Name of the Pythia parameter file to read");
  declareOutput(  "hepmc"   , m_hepmchandle);

  declareProperty("PileUpTool", m_pileUpTool);
  declarePrivateTool(m_pileUpTool, "ConstPileUp/PileUpTool");
}

StatusCode PythiaInterface::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc.isSuccess()) return sc;
  if ( m_parfile.empty() ) { return Error ( "Define Pythia8 configuration file (*.cmd)!" ); }

  // Set Pythia configuration directory from system variable (if set)
  std::string xmlpath = "" ;
  if (System::getEnv("PYTHIA8_XML") != "UNKNOWN") xmlpath = System::getEnv("PYTHIA8_XML") ;

  // Initialize pythia
  m_pythiaSignal = std::unique_ptr<Pythia8::Pythia>(new Pythia8::Pythia(xmlpath));
  m_pythiaPileup = std::unique_ptr<Pythia8::Pythia>(new Pythia8::Pythia(xmlpath));

  // Read Pythia configuration files
  m_pythiaSignal->readFile( m_parfile.c_str() );
  // do not bother with pileup configuration if no pileup
  if (0. < m_pileUpTool->getMeanPileUp()) {
    if (m_pileUpTool->getFilename().empty()) {
     return Error ( "Define Pythia8 configuration file for pileup in pileuptool (*.cmd)!" );  
    }
    m_pythiaPileup->readFile(m_pileUpTool->getFilename().c_str());
  }
  m_pythiaPileup->init();

  // Initialize variables from configuration file
  m_nAbort = m_pythiaSignal->settings.mode("Main:timesAllowErrors"); // how many aborts before run stops
  m_iAbort = 0;
  m_iEvent = 0;

  // begin ME/PS Matching specific code
  
  // Check if jet matching should be applied.
  m_doMePsMatching = m_pythiaSignal->settings.flag("JetMatching:merge");

  // Check if internal merging should be applied.
  m_doMePsMerging = !(m_pythiaSignal->settings.word("Merging:Process").compare("void") == 0);

  // Currently, only one scheme at a time is allowed.
  if (m_doMePsMerging && m_doMePsMatching) {
    return Error ( "Jet matching and merging cannot be used simultaneously!" );
  }
  
  // Allow to set the number of addtional partons dynamically.
  if (m_doMePsMerging) {
    // Store merging scheme.
    int scheme = ( m_pythiaSignal->settings.flag("Merging:doUMEPSTree")
                || m_pythiaSignal->settings.flag("Merging:doUMEPSSubt")) ?
                1 :
                 ( ( m_pythiaSignal->settings.flag("Merging:doUNLOPSTree")
                || m_pythiaSignal->settings.flag("Merging:doUNLOPSSubt")
                || m_pythiaSignal->settings.flag("Merging:doUNLOPSLoop")
                || m_pythiaSignal->settings.flag("Merging:doUNLOPSSubtNLO")) ?
                2 :
                0 );
//    m_setting = std::unique_ptr<Pythia8::amcnlo_unitarised_interface>(new Pythia8::amcnlo_unitarised_interface(scheme));
    m_setting = new Pythia8::amcnlo_unitarised_interface(scheme);
    m_pythiaSignal->setUserHooksPtr(m_setting);
  }

  // For jet matching, initialise the respective user hooks code.
  if (m_doMePsMatching) {
    m_matching = new Pythia8::JetMatchingMadgraph();
    if (!m_matching) {
      return Error ( " Failed to initialise jet matching structures.");
    }
    m_pythiaSignal->setUserHooksPtr(m_matching);
  }
  
  if(m_doMePsMatching || m_doMePsMerging) declareOutput("mePsMatchingVars" , m_handleMePsMatchingVars, "mePsMatchingVars");
  
  // end ME/PS Matching specific code

  m_pythiaSignal->init();

  // Return the status code
  return sc;
}

StatusCode PythiaInterface::execute() {

  // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC *toHepMC = new HepMC::Pythia8ToHepMC();
  Pythia8::Event sumEvent;
 
  // Generate events. Quit if many failures in a row
  while ( !m_pythiaSignal->next() ) {
    if (++m_iAbort > m_nAbort) {

      IIncidentSvc* incidentSvc;
      service("IncidentSvc",incidentSvc);
      incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
      return Error ( "Event generation aborted prematurely, owing to error!" );
    }
    else{
       warning () << "PythiaInterface Pythia8 abort : "<< m_iAbort << "/" << m_nAbort << std::endl;
    }
    
    /*std::vector<double> my_DJRs = matching->getDJR();
    int my_nHardPartons    = matching->nMEpartons().first;

    double evtweight         = m_pythiaSignal->info.weight();
    if (m_doMePsMerging) evtweight  *= m_pythiaSignal->info.mergingWeightNLO()*setting->getNormFactor();
    
    // Do not print zero-weight events.
    if ( evtweight == 0. ) continue;
    // end ME/PS Matching specific code */
    
    
    
  }

  if(m_doMePsMatching || m_doMePsMerging) {

    auto mePsMatchingVars = m_handleMePsMatchingVars.createAndPut();

 
    double ptprint = m_pythiaSignal->settings.parm("Syscalc:qWeed");
    //double ptmin = (doMerge) ? pythia.settings.parm("Merging:TMS") : 10.0;
    Pythia8::SlowJet* slowJet = new Pythia8::SlowJet(1, 0.4, ptprint, 4.4, 2, 2, NULL, false);
  
    int njetNow = 0;
    std::vector<double> dijVec;
    // Construct input for jet algorithm.
    Pythia8::Event jetInput;
    jetInput.init("jet input",&(m_pythiaSignal->particleData));
    jetInput.clear();
    for (int i =0; i < m_pythiaSignal->event.size(); ++i)
      if (  m_pythiaSignal->event[i].isFinal()
        && (m_pythiaSignal->event[i].colType() != 0 || m_pythiaSignal->event[i].isHadron()))
        jetInput.append(m_pythiaSignal->event[i]);
    slowJet->setup(jetInput);
    // Run jet algorithm.
    std::vector<double> result;
    while ( slowJet->sizeAll() - slowJet->sizeJet() > 0 ) {
      result.push_back(sqrt(slowJet->dNext()));
      slowJet->doStep();
    }
    // Reorder by decreasing multiplicity.
    for (int i=int(result.size())-1; i >= 0; --i)
      dijVec.push_back(result[i]);

    // Now get the "number of partons" in the input event, so that
    // we may tag this event accordingly when histogramming. Note
    // that for MLM jet matching, this might not coincide with the
    // actual number of partons in the input LH event, since some
    // partons may be excluded from the matching.
    if (m_doMePsMatching) njetNow = m_matching->nMEpartons().first;
    else if (m_doMePsMerging){
      njetNow = m_pythiaSignal->settings.mode("Merging:nRequested");
      if ( m_pythiaSignal->settings.flag("Merging:doUMEPSSubt")
        || m_pythiaSignal->settings.flag("Merging:doUNLOPSSubt")
        || m_pythiaSignal->settings.flag("Merging:doUNLOPSSubtNLO") )
        njetNow--;
    }

    // Inclusive jet pTs as further validation plot.
    std::vector<double> ptVec;
    // Run jet algorithm.
    slowJet->analyze(jetInput);
    for (int i = 0; i < slowJet->sizeJet(); ++i)
      ptVec.push_back(slowJet->pT(i));

    // end matching code from MG5
    /*std::cout<<"------------------------------  main method ----------------------------------"<<std::endl;
    std::cout<<"nPartons ME: "<<njetNow<<std::endl;
    std::cout<<"d01: "<<log10(dijVec[0])<<", d12: "<<log10(dijVec[1])<<", d23: "<<log10(dijVec[2])<<", d34: "<<log10(dijVec[3])<<std::endl;
    std::cout<<"pt1: "<<ptVec[0]<<", pt2: "<<ptVec[1]<<", pt3: "<<ptVec[2]<<", pt4: "<<ptVec[3]<<std::endl;
    */
    
    auto var = mePsMatchingVars->create();
    var.value(njetNow);
    
    for (int i = 0; i < 4; ++i) {
    
      var = mePsMatchingVars->create();
      var.value(-999);
      if(dijVec.size() > i)
        var.value(log10(dijVec[i]));
      
      var = mePsMatchingVars->create();
      var.value(-999);
      if(ptVec.size() > i)
        var.value(ptVec[i]);
    }
  }
  // begin ME/PS Matching specific code
/*  std::cout<<m_doMePsMatching<<std::endl;
  if(m_doMePsMatching || m_doMePsMerging) {
      
    auto mePsMatchingVars = m_handleMePsMatchingVars.createAndPut();
    auto var = mePsMatchingVars->create();
    var.value(m_matching->nMEpartons().first);
    //loop over HepMC event weights
    // FIXME: method becomes "getDJR()" starting from Pythia8.219
    for(unsigned int j=0; j<m_matching->GetDJR().size(); j++) {
      auto var = mePsMatchingVars->create();
      var.value(m_matching->GetDJR()[j]);
    }
    std::cout<<"-------------------------------------------------------------------"<<std::endl;
    std::cout<<"nPartons ME: "<<m_matching->nMEpartons().first<<std::endl;
    std::cout<<"d01: "<<log10(m_matching->GetDJR()[0])<<", d12: "<<log10(m_matching->GetDJR()[1])<<", d23: "<<log10(m_matching->GetDJR()[2])<<", d34: "<<log10(m_matching->GetDJR()[3])<<std::endl;
  }
*/

  // Reset the counter to count failed events in a row
  m_iAbort=0;

  // Print debug: Pythia event info
  if (msgLevel() <= MSG::DEBUG) {

    for (int i = 0; i < m_pythiaSignal->event.size(); ++i){
      info () << "PythiaInterface Pythia8 abort : "<< m_iAbort << "/" << m_nAbort << endmsg;

      debug() << "Pythia: "
              << " Id: "        << std::setw(3) << i
              << " PDG: "       << std::setw(5) << m_pythiaSignal->event[i].id()
              << " Mothers: "   << std::setw(3) << m_pythiaSignal->event[i].mother1()   << " -> " << std::setw(3) << m_pythiaSignal->event[i].mother2()
              << " Daughters: " << std::setw(3) << m_pythiaSignal->event[i].daughter1() << " -> " << std::setw(3) << m_pythiaSignal->event[i].daughter2()
              << " Stat: "      << std::setw(2) << m_pythiaSignal->event[i].status()
              << std::scientific
              << std::setprecision(2) << " Px: " << std::setw(9) << m_pythiaSignal->event[i].px()
              << std::setprecision(2) << " Py: " << std::setw(9) << m_pythiaSignal->event[i].py()
              << std::setprecision(2) << " Pz: " << std::setw(9) << m_pythiaSignal->event[i].pz()
              << std::setprecision(2) << " E: "  << std::setw(9) << m_pythiaSignal->event[i].e()
              << std::setprecision(2) << " M: "  << std::setw(9) << m_pythiaSignal->event[i].m()
              << std::fixed
              << endmsg;
    }
  } // Debug

  // Generate a number of pileup events.
  for (unsigned int iPileup = 0; iPileup < m_pileUpTool->numberOfPileUp(); ++iPileup) {
    m_pythiaPileup->next();
    // add pileup to signal
    m_pythiaSignal->event += m_pythiaPileup->event;
  }

  // Define HepMC event and convert Pythia event into this HepMC event type
  auto theEvent = new HepMC::GenEvent( gen::hepmcdefault::energy, gen::hepmcdefault::length );
  toHepMC->fill_next_event(*m_pythiaSignal, theEvent, m_iEvent);

  // Print debug: HepMC event info
  if (msgLevel() <= MSG::DEBUG) {

    for (auto ipart = theEvent->particles_begin(); ipart!=theEvent->particles_end(); ++ipart) {

      int motherID        = -1;
      int motherIDRange   = 0;
      if ((*ipart)->production_vertex()!=nullptr) {

        motherID      = (*((*ipart)->production_vertex()->particles_in_const_begin()))->barcode();
        motherIDRange = (*ipart)->production_vertex()->particles_in_size() -1;
      }

      int daughterID      = -1;
      int daughterIDRange = 0;
      if ((*ipart)->end_vertex()!=nullptr) {

        daughterID      = (*((*ipart)->end_vertex()->particles_out_const_begin()))->barcode();
        daughterIDRange = (*ipart)->end_vertex()->particles_out_size() -1;
      }

      debug() << "HepMC: "
              << " Id: "       << std::setw(3)  << (*ipart)->barcode()
              << " Pdg: "      << std::setw(5)  << (*ipart)->pdg_id()
              << " Mothers: "  << std::setw(3)  << motherID   << " -> " << std::setw(3) << motherID+motherIDRange
              << " Daughters: "<< std::setw(3)  << daughterID << " -> " << std::setw(3) << daughterID+daughterIDRange
              << " Stat: "     << std::setw(2)  << (*ipart)->status()
              << std::scientific
              << " Px: "       << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().px()
              << " Py: "       << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().py()
              << " Pz: "       << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().pz()
              << " E: "        << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().e()
              << " M: "        << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().m()
              << std::fixed;
      if ((*ipart)->production_vertex()!=nullptr) {
      debug() << std::scientific
              << " Vx: "       << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().x()
              << " Vy: "       << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().y()
              << " Vz: "       << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().z()
              << " T: "        << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().t()
              << std::fixed;
      }
      debug() << endmsg;
    }
  } // Debug

  // Handle event via standard Gaudi mechanism
  m_hepmchandle.put(theEvent);
  m_iEvent++;
  return StatusCode::SUCCESS;
}

StatusCode PythiaInterface::finalize() {

  m_pythiaSignal.reset();
  m_pythiaPileup.reset();
  return GaudiAlgorithm::finalize();
}
