#include "HepMCReader.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

DECLARE_COMPONENT(HepMCReader)

HepMCReader::HepMCReader(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc),
  m_filename()
{
  declareProperty("Filename", m_filename="",
                  "Name of the HepMC file to read");
  declareOutput("hepmc", m_hepmchandle);
}

StatusCode HepMCReader::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc.isSuccess()) return sc;

  if ( m_filename.empty() )
    { return Error ( "Input file name is not specified!" ) ; }
  // open the file
  m_file = new HepMC::IO_GenEvent ( m_filename.c_str() , std::ios::in ) ;
  //  
  if ( ( 0 == m_file ) || ( m_file->rdstate() == std::ios::failbit ) )
    { return Error ( "Failure to read the file '"+m_filename+"'" ) ; }
  //

  return sc;
}

StatusCode HepMCReader::execute() {
  HepMC::GenEvent* theEvent = new HepMC::GenEvent();
  Assert ( 0 != m_file , "Invalid input file!" );
  if ( !m_file->fill_next_event( theEvent ) ) {
    IIncidentSvc* incidentSvc;
    service("IncidentSvc",incidentSvc);
    incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
    if ( m_file -> rdstate() == std::ios::eofbit )
        return Error ( "Error in event reading!" ) ;
      else return Error( "No more events in input file, set correct number of events in options" ) ;
      ;
    }
  HepMCEntry * entry = new HepMCEntry();
  entry->setEvent(theEvent);
  m_hepmchandle.put(entry);
  return StatusCode::SUCCESS;
}

StatusCode HepMCReader::finalize() {
  if ( 0 != m_file ) { delete m_file ; m_file = 0 ; }
  return GaudiAlgorithm::finalize();
}
