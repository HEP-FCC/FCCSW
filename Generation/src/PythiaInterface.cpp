#include "GaudiKernel/System.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "PythiaInterface.h"

#include "HepMC/GenEvent.h"

DECLARE_COMPONENT(PythiaInterface)

PythiaInterface::PythiaInterface(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc), m_pythia( nullptr ), m_parfile()
{
  declareProperty("Filename", m_parfile="", "Name of the Pythia parameter file to read");
  declareOutput(  "hepmc"   , m_hepmchandle);
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

  // Print out event
  /*for (int i = 0; i < m_pythia->event.size(); ++i){
      //if (m_pythia->event[i].isFinal() && m_pythia->event[i].isCharged())
      std::cout << "PythiaInterface : id : stat : px : py : pz : e : m : " 
                << " " << m_pythia->event[i].id()
                << " " << m_pythia->event[i].status()
                //<< " " << m_pythia->event[i].mother1()
                //<< " " << m_pythia->event[i].mother2()
                //<< " " << m_pythia->event[i].daughter1()
                //<< " " << m_pythia->event[i].daughter2()
                << " " << m_pythia->event[i].px()
                << " " << m_pythia->event[i].py()
                << " " << m_pythia->event[i].pz() 
                << " " << m_pythia->event[i].e()
                << " " << m_pythia->event[i].m()
                << std::endl;
  }*/

  // Define HepMC event and convert Pythia event into this HepMC event type
  HepMC::GenEvent* theEvent = new HepMC::GenEvent( HepMC::Units::GEV, HepMC::Units::MM);
  toHepMC->fill_next_event(*m_pythia, theEvent, m_iEvent);
  //theEvent-> print();

  // Print out event
  /*for (HepMC::GenEvent::particle_iterator ipart = theEvent->particles_begin() ;
       ipart!=theEvent->particles_end(); ++ipart) {

    int motherID                = -1;
    int motherIDRange           = 0;
    if ((*ipart)->production_vertex()!=nullptr) {

      motherID      = (*((*ipart)->production_vertex()->particles_in_const_begin()))->barcode();//(*((*ipart)->production_vertex()->particles_begin()))->barcode();
      motherIDRange = (*ipart)->production_vertex()->particles_in_size() -1;
    }

    std::cout << "Pythia HepMC: "
              << " Id: "       << setw(3)  << (*ipart)->barcode()
              << " Pdg: "      << setw(5)  << (*ipart)->pdg_id()
              << " Mothers: "  << setw(3)  << motherID << " -> " << setw(3) << motherID+motherIDRange
              << " Stat: "     << setw(2)  << (*ipart)->status()
              << " Px: "       << setw(10) << (*ipart)->momentum().px()
              << " Py: "       << setw(10) << (*ipart)->momentum().py()
              << " Pz: "       << setw(10) << (*ipart)->momentum().pz()
              << " E: "        << setw(10) << (*ipart)->momentum().e()
              << " M: "        << setw(10) << (*ipart)->momentum().m();
    if ((*ipart)->production_vertex()!=nullptr) {
    std::cout << " Vx: "       << setw(10) << (*ipart)->production_vertex()->position().x()
              << " Vy: "       << setw(10) << (*ipart)->production_vertex()->position().y()
              << " Vz: "       << setw(10) << (*ipart)->production_vertex()->position().z()
              << " T: "        << setw(10) << (*ipart)->production_vertex()->position().t();
    }
    std::cout << std::endl;
  }*/

  // Handle event via standard Gaudi mechanism
  m_hepmchandle.put(theEvent);
  m_iEvent++;
  return StatusCode::SUCCESS;
}

StatusCode PythiaInterface::finalize() {
  if ( m_pythia != nullptr ) { delete m_pythia ; m_pythia = nullptr; }
  return GaudiAlgorithm::finalize();
}
