#include "PythiaInterface.h"

// Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/System.h"

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

#include "HepMC/GenEvent.h"

DECLARE_TOOL_FACTORY(PythiaInterface)

PythiaInterface::PythiaInterface(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent),
      m_pythiaSignal(nullptr),
      m_nAbort(0),
      m_iAbort(0),
      m_iEvent(0),
      m_doMePsMatching(0),
      m_doMePsMerging(0),
      m_matching(nullptr),
      m_setting(nullptr) {

  declareProperty("VertexSmearingTool", m_vertexSmearingTool);
  declarePrivateTool(m_vertexSmearingTool, "FlatSmearVertex/VertexSmearingTool");
}

StatusCode PythiaInterface::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (!sc.isSuccess()) return sc;
  if (m_parfile.empty()) {
    return Error("Define Pythia8 configuration file (*.cmd)!");
  }

  // Set Pythia configuration directory from system variable (if set)
  std::string xmlpath = "";
  if (System::getEnv("PYTHIA8_XML") != "UNKNOWN") xmlpath = System::getEnv("PYTHIA8_XML");

  // Initialize pythia
  m_pythiaSignal = std::unique_ptr<Pythia8::Pythia>(new Pythia8::Pythia(xmlpath));

  // Read Pythia configuration files
  m_pythiaSignal->readFile(m_parfile.value().c_str());
  // do not bother with pileup configuration if no pileup

  // Initialize variables from configuration file
  m_nAbort = m_pythiaSignal->settings.mode("Main:timesAllowErrors");  // how many aborts before run stops
  m_iAbort = 0;
  m_iEvent = 0;

  // Begin ME/PS Matching specific code
  // Check if jet matching should be applied.
  m_doMePsMatching = m_pythiaSignal->settings.flag("JetMatching:merge");

  // Check if internal merging should be applied.
  m_doMePsMerging = !(m_pythiaSignal->settings.word("Merging:Process").compare("void") == 0);

  // Currently, only one scheme at a time is allowed.
  if (m_doMePsMerging && m_doMePsMatching) {
    return Error("Jet matching and merging cannot be used simultaneously!");
  }

  // Allow to set the number of additional partons dynamically.
  if (m_doMePsMerging) {
    // Store merging scheme.
    int scheme;
    if (m_pythiaSignal->settings.flag("Merging:doUMEPSTree") || m_pythiaSignal->settings.flag("Merging:doUMEPSSubt")) {
      scheme = 1;
    } else if (m_pythiaSignal->settings.flag("Merging:doUNLOPSTree") ||
               m_pythiaSignal->settings.flag("Merging:doUNLOPSSubt") ||
               m_pythiaSignal->settings.flag("Merging:doUNLOPSLoop") ||
               m_pythiaSignal->settings.flag("Merging:doUNLOPSSubtNLO")) {
      scheme = 2;
    } else {
      scheme = 0;
    }

    m_setting = std::unique_ptr<Pythia8::amcnlo_unitarised_interface>(new Pythia8::amcnlo_unitarised_interface(scheme));
    m_pythiaSignal->setUserHooksPtr(m_setting.get());
  }

  // For jet matching, initialise the respective user hooks code.
  if (m_doMePsMatching) {
    m_matching = std::unique_ptr<Pythia8::JetMatchingMadgraph>(new Pythia8::JetMatchingMadgraph());
    if (!m_matching) {
      return Error(" Failed to initialise jet matching structures.");
    }
    m_pythiaSignal->setUserHooksPtr(m_matching.get());
  }

  // jet clustering needed for matching
  m_slowJet = std::unique_ptr<Pythia8::SlowJet>(new Pythia8::SlowJet(1, 0.4, 0, 4.4, 2, 2, NULL, false));

  // End ME/PS Matching specific code

  m_pythiaSignal->init();

  // Return the status code
  return sc;
}

StatusCode PythiaInterface::getNextEvent(HepMC::GenEvent& theEvent) {

  // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC* toHepMC = new HepMC::Pythia8ToHepMC();
  Pythia8::Event sumEvent;

  // Generate events. Quit if many failures in a row
  while (!m_pythiaSignal->next()) {
    if (++m_iAbort > m_nAbort) {

      IIncidentSvc* incidentSvc;
      service("IncidentSvc", incidentSvc);
      incidentSvc->fireIncident(Incident(name(), IncidentType::AbortEvent));
      return Error("Event generation aborted prematurely, owing to error!");
    } else {
      warning() << "PythiaInterface Pythia8 abort : " << m_iAbort << "/" << m_nAbort << std::endl;
    }
  }

  if (m_doMePsMatching || m_doMePsMerging) {

    auto mePsMatchingVars = m_handleMePsMatchingVars.createAndPut();
    int njetNow = 0;
    std::vector<double> dijVec;

    // Construct input for jet algorithm.
    Pythia8::Event jetInput;
    jetInput.init("jet input", &(m_pythiaSignal->particleData));
    jetInput.clear();
    for (int i = 0; i < m_pythiaSignal->event.size(); ++i)
      if (m_pythiaSignal->event[i].isFinal() &&
          (m_pythiaSignal->event[i].colType() != 0 || m_pythiaSignal->event[i].isHadron()))
        jetInput.append(m_pythiaSignal->event[i]);
    m_slowJet->setup(jetInput);
    // Run jet algorithm.
    std::vector<double> result;
    while (m_slowJet->sizeAll() - m_slowJet->sizeJet() > 0) {
      result.push_back(sqrt(m_slowJet->dNext()));
      m_slowJet->doStep();
    }

    // Reorder by decreasing multiplicity.
    for (int i = int(result.size()) - 1; i >= 0; --i)
      dijVec.push_back(result[i]);

    // Now get the "number of partons" in the input event, so that
    // we may tag this event accordingly when histogramming. Note
    // that for MLM jet matching, this might not coincide with the
    // actual number of partons in the input LH event, since some
    // partons may be excluded from the matching.

    bool doShowerKt = m_pythiaSignal->settings.flag("JetMatching:doShowerKt");
    if (m_doMePsMatching && !doShowerKt) njetNow = m_matching->nMEpartons().first;
    // FIXME: "getProcessSubset()" method does not exist in < 8.219
    // FIXME: simply un-comment the following two lines in >= 8.219
    // else if (m_doMePsMatching && doShowerKt)
    // njetNow = m_matching->getProcessSubset().size();
    else if (m_doMePsMerging) {
      njetNow = m_pythiaSignal->settings.mode("Merging:nRequested");
      if (m_pythiaSignal->settings.flag("Merging:doUMEPSSubt") ||
          m_pythiaSignal->settings.flag("Merging:doUNLOPSSubt") ||
          m_pythiaSignal->settings.flag("Merging:doUNLOPSSubtNLO"))
        njetNow--;
    }

    // Inclusive jet pTs as further validation plot.
    std::vector<double> ptVec;
    // Run jet algorithm.
    m_slowJet->analyze(jetInput);
    for (int i = 0; i < m_slowJet->sizeJet(); ++i)
      ptVec.push_back(m_slowJet->pT(i));

    auto var = mePsMatchingVars->create();

    // 0th entry = number of generated partons
    var.value(njetNow);

    // odd  entries: d(ij) observables --- 1): d01, 3): d12, 5): d23, 7): d34
    // even entries: pT(i) observables --- 2): pT1, 4): pT2, 6): pT3, 8): pT4
    for (unsigned int i = 0; i < 4; ++i) {
      var = mePsMatchingVars->create();
      var.value(-999);
      if (dijVec.size() > i) var.value(log10(dijVec[i]));

      var = mePsMatchingVars->create();
      var.value(-999);
      if (ptVec.size() > i) var.value(ptVec[i]);
    }
  }

  // Reset the counter to count failed events in a row
  m_iAbort = 0;

  // Print debug: Pythia event info
  if (msgLevel() <= MSG::DEBUG) {

    for (int i = 0; i < m_pythiaSignal->event.size(); ++i) {
      debug() << "PythiaInterface Pythia8 abort : " << m_iAbort << "/" << m_nAbort << endmsg;

      debug() << "Pythia: "
              << " Id: " << std::setw(3) << i << " PDG: " << std::setw(5) << m_pythiaSignal->event[i].id()
              << " Mothers: " << std::setw(3) << m_pythiaSignal->event[i].mother1() << " -> " << std::setw(3)
              << m_pythiaSignal->event[i].mother2() << " Daughters: " << std::setw(3)
              << m_pythiaSignal->event[i].daughter1() << " -> " << std::setw(3) << m_pythiaSignal->event[i].daughter2()
              << " Stat: " << std::setw(2) << m_pythiaSignal->event[i].status() << std::scientific
              << std::setprecision(2) << " Px: " << std::setw(9) << m_pythiaSignal->event[i].px()
              << std::setprecision(2) << " Py: " << std::setw(9) << m_pythiaSignal->event[i].py()
              << std::setprecision(2) << " Pz: " << std::setw(9) << m_pythiaSignal->event[i].pz()
              << std::setprecision(2) << " E: " << std::setw(9) << m_pythiaSignal->event[i].e() << std::setprecision(2)
              << " M: " << std::setw(9) << m_pythiaSignal->event[i].m() << std::fixed << endmsg;
    }
  }  // Debug

  // Define HepMC event and convert Pythia event into this HepMC event type
  toHepMC->fill_next_event(*m_pythiaSignal, &theEvent, m_iEvent);

  // Print debug: HepMC event info
  if (msgLevel() <= MSG::DEBUG) {

    for (auto ipart = theEvent.particles_begin(); ipart != theEvent.particles_end(); ++ipart) {

      int motherID = -1;
      int motherIDRange = 0;
      if ((*ipart)->production_vertex() != nullptr) {

        motherID = (*((*ipart)->production_vertex()->particles_in_const_begin()))->barcode();
        motherIDRange = (*ipart)->production_vertex()->particles_in_size() - 1;
      }

      int daughterID = -1;
      int daughterIDRange = 0;
      if ((*ipart)->end_vertex() != nullptr) {

        daughterID = (*((*ipart)->end_vertex()->particles_out_const_begin()))->barcode();
        daughterIDRange = (*ipart)->end_vertex()->particles_out_size() - 1;
      }

      debug() << "HepMC: "
              << " Id: " << std::setw(3) << (*ipart)->barcode() << " Pdg: " << std::setw(5) << (*ipart)->pdg_id()
              << " Mothers: " << std::setw(3) << motherID << " -> " << std::setw(3) << motherID + motherIDRange
              << " Daughters: " << std::setw(3) << daughterID << " -> " << std::setw(3) << daughterID + daughterIDRange
              << " Stat: " << std::setw(2) << (*ipart)->status() << std::scientific << " Px: " << std::setprecision(2)
              << std::setw(9) << (*ipart)->momentum().px() << " Py: " << std::setprecision(2) << std::setw(9)
              << (*ipart)->momentum().py() << " Pz: " << std::setprecision(2) << std::setw(9)
              << (*ipart)->momentum().pz() << " E: " << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().e()
              << " M: " << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().m() << std::fixed;
      if ((*ipart)->production_vertex() != nullptr) {
        debug() << std::scientific << " Vx: " << std::setprecision(2) << std::setw(9)
                << (*ipart)->production_vertex()->position().x() << " Vy: " << std::setprecision(2) << std::setw(9)
                << (*ipart)->production_vertex()->position().y() << " Vz: " << std::setprecision(2) << std::setw(9)
                << (*ipart)->production_vertex()->position().z() << " T: " << std::setprecision(2) << std::setw(9)
                << (*ipart)->production_vertex()->position().t() << std::fixed;
      }
      debug() << endmsg;
    }
  }  // Debug

  // Handle event via standard Gaudi mechanism
  m_iEvent++;

  delete toHepMC;
  return StatusCode::SUCCESS;
}

StatusCode PythiaInterface::finalize() {

  m_pythiaSignal.reset();
  return GaudiTool::finalize();
}
