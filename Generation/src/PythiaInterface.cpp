#include "PythiaInterface.h"

// Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"

// Pythia
#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

using namespace std;

DECLARE_COMPONENT(PythiaInterface)

PythiaInterface::PythiaInterface(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc), m_pythia( nullptr ), m_parfile()
{
  declareProperty("Filename", m_parfile="", "Name of the Pythia parameter file to read");
  declareOutput(  "hepmc"   , m_hepmchandle);

  m_nAbort = 0;
  m_iAbort = 0;
  m_iEvent = 0;
}

StatusCode PythiaInterface::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc.isSuccess()) return sc;
  if ( m_parfile.empty() ) { return Error ( "Define Pythia8 configuration file (*.cmd)!" ); }

  // Set Pythia configuration directory from system variable (if set)
  std::string xmlpath = "" ;
  if (System::getEnv("PYTHIA8_XML") != "UNKNOWN") xmlpath = System::getEnv("PYTHIA8_XML") ;

  // Initialize pythia
  m_pythia = new Pythia8::Pythia ( xmlpath );

  // Read Pythia configuration file
  m_pythia->readFile( m_parfile.c_str() );

  // Initialize variables from configuration file
  m_nAbort = m_pythia->settings.mode("Main:timesAllowErrors"); // how many aborts before run stops
  m_iAbort = 0;
  m_iEvent = 0;

  m_pythia->init();

  // Return the status code
  return sc;
}

StatusCode PythiaInterface::execute() {

  // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC *toHepMC = new HepMC::Pythia8ToHepMC();

  // Generate events. Quit if many failures in a row
  while ( !m_pythia->next() ) {
    if (++m_iAbort > m_nAbort) {

      IIncidentSvc* incidentSvc;
      service("IncidentSvc",incidentSvc);
      incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
      return Error ( "Event generation aborted prematurely, owing to error!" );
    }
    else{
       warning () << "PythiaInterface Pythia8 abort : "<< m_iAbort << "/" << m_nAbort << std::endl;
    }
  }

  // Reset the counter to count failed events in a row
  m_iAbort=0;

  // Print debug: Pythia event info
  if (msgLevel() <= MSG::DEBUG) {

    for (int i = 0; i < m_pythia->event.size(); ++i){

      //if (m_pythia->event[i].isFinal() && m_pythia->event[i].isCharged())
      std::cout << "PythiaInterface : "
                << " Id: "       << setw(3) << i
                << " PDG: "      << setw(5) << m_pythia->event[i].id()
                << " Mothers: "  << setw(3) << m_pythia->event[i].mother1()   << " -> " << setw(3) << m_pythia->event[i].mother2()
                << " Daughters:" << setw(3) << m_pythia->event[i].daughter1() << " -> " << setw(3) << m_pythia->event[i].daughter2()
                << " Stat: "     << setw(2) << m_pythia->event[i].status()
                << setprecision(2) << " Px: " << setw(9) << m_pythia->event[i].px()
                << setprecision(2) << " Py: " << setw(9) << m_pythia->event[i].py()
                << setprecision(2) << " Pz: " << setw(9) << m_pythia->event[i].pz()
                << setprecision(2) << " E: "  << setw(9) << m_pythia->event[i].e()
                << setprecision(2) << " M: "  << setw(9) << m_pythia->event[i].m()
                << std::endl;
    }
  } // Debug

  // Define HepMC event and convert Pythia event into this HepMC event type
  HepMC::GenEvent* theEvent = new HepMC::GenEvent( HepMC::Units::GEV, HepMC::Units::MM);
  toHepMC->fill_next_event(*m_pythia, theEvent, m_iEvent);
  //theEvent-> print();

  // Print debug: HepMC event info
  if (msgLevel() <= MSG::DEBUG) {

    for (HepMC::GenEvent::particle_iterator ipart = theEvent->particles_begin() ;
         ipart!=theEvent->particles_end(); ++ipart) {

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

      std::cout << "Pythia HepMC: "
                << " Id: "       << setw(3)  << (*ipart)->barcode()
                << " Pdg: "      << setw(5)  << (*ipart)->pdg_id()
                << " Mothers: "  << setw(3)  << motherID << " -> " << setw(3) << motherID+motherIDRange
                << " Daughters: "<< setw(3)  << daughterID << " -> " << setw(3) << daughterID+daughterIDRange
                << " Stat: "     << setw(2)  << (*ipart)->status()
                << " Px: "       << setprecision(2) << setw(9) << (*ipart)->momentum().px()
                << " Py: "       << setprecision(2) << setw(9) << (*ipart)->momentum().py()
                << " Pz: "       << setprecision(2) << setw(9) << (*ipart)->momentum().pz()
                << " E: "        << setprecision(2) << setw(9) << (*ipart)->momentum().e()
                << " M: "        << setprecision(2) << setw(9) << (*ipart)->momentum().m();
      if ((*ipart)->production_vertex()!=nullptr) {
      std::cout << " Vx: "       << setprecision(2) << setw(9) << (*ipart)->production_vertex()->position().x()
                << " Vy: "       << setprecision(2) << setw(9) << (*ipart)->production_vertex()->position().y()
                << " Vz: "       << setprecision(2) << setw(9) << (*ipart)->production_vertex()->position().z()
                << " T: "        << setprecision(2) << setw(9) << (*ipart)->production_vertex()->position().t();
      }
      std::cout << std::endl;
    }
  } // Debug

  // Handle event via standard Gaudi mechanism
  m_hepmchandle.put(theEvent);
  m_iEvent++;
  return StatusCode::SUCCESS;
}

StatusCode PythiaInterface::finalize() {
  if ( m_pythia != nullptr ) { delete m_pythia ; m_pythia = nullptr; }
  return GaudiAlgorithm::finalize();
}
