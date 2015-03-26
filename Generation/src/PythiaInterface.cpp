#include "GaudiKernel/System.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "PythiaInterface.h"
#include "Pythia8/Pythia.h"
#include "Pythia8/Pythia8ToHepMC.h"

#include "HepMC/GenEvent.h"

DECLARE_COMPONENT(PythiaInterface)

PythiaInterface::PythiaInterface(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc), m_pythia( nullptr ), m_parfile()
{
  declareProperty("Filename", m_parfile="", "Name of the Pythia parameter file to read");
  declareOutput("hepmc", m_hepmchandle);
}

StatusCode PythiaInterface::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc.isSuccess()) return sc;
  if ( m_parfile.empty() )
    { return Error ( "Define Pythia8 parameter file!" ); }

  /// initialize with path
  std::string xmlpath = "" ;
  if ( "UNKNOWN" != System::getEnv( "PYTHIA8_XML" ) ) 
    xmlpath  = System::getEnv( "PYTHIA8_XML" ) ;
  m_pythia = new Pythia8::Pythia ( xmlpath );
  /// read command file
  m_pythia->readFile( m_parfile.c_str() );
  // initial settings from param file
  nAbort = m_pythia->settings.mode("Main:timesAllowErrors"); // how many aborts before run stops
  m_pythia->init();
  return sc;
}

StatusCode PythiaInterface::execute() {

  /// Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC *toHepMC = new HepMC::Pythia8ToHepMC();

  /// Generate events. Quit if many failures
  if ( !m_pythia->next() ) {
    IIncidentSvc* incidentSvc;
    service("IncidentSvc",incidentSvc);
    incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
    return Error ( "Event generation aborted prematurely, owing to error!" );
  }

  /*
  for (int i = 0; i < m_pythia->event.size(); ++i){ 
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
  }
  */

  /// Construct new empty HepMC event
  HepMC::GenEvent* theEvent = new HepMC::GenEvent( HepMC::Units::GEV, HepMC::Units::MM);
  toHepMC->fill_next_event(*m_pythia,theEvent);

  /*
  for (HepMC::GenEvent::particle_iterator ipart = theEvent->particles_begin() ;
       ipart!=theEvent->particles_end(); ++ipart)
       std::cout << "HepMC : id : stat : px : py : pz : e : m : " 
              << (*ipart)->pdg_id()
              << " " << (*ipart)->status()
              << " " << (*ipart)->momentum().px()
              << " " << (*ipart)->momentum().py()
              << " " << (*ipart)->momentum().pz() 
              << " " << (*ipart)->momentum().e()
              << " " << (*ipart)->momentum().m()
              << std::endl;
  */

  HepMCEntry * entry = new HepMCEntry();
  entry->setEvent(theEvent);
  m_hepmchandle.put(entry);
  return StatusCode::SUCCESS;
}

StatusCode PythiaInterface::finalize() {
  if ( 0 != m_pythia ) { delete m_pythia ; m_pythia = 0; }
  return GaudiAlgorithm::finalize();
}
