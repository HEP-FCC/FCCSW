#include "PoissonPileUp.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

DECLARE_TOOL_FACTORY( PoissonPileUp )

PoissonPileUp::PoissonPileUp( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
  declareInterface< IPileUpTool >( this );
  
 declareProperty( "numPileUpEvents", m_meanPileUpEvents = 0 );
 declareProperty( "Filename", m_filename = "");
}


PoissonPileUp::~PoissonPileUp( ) { ; }


StatusCode PoissonPileUp::initialize( ) {
  StatusCode sc = GaudiTool::initialize( );
  if ( sc.isFailure() ) return sc ;
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  if ( m_meanPileUpEvents < 0 ) return Error( "Number of Pileup events cannot be negative!" ) ;
  sc = m_PoissonDist.initialize( randSvc , Rndm::Poisson( m_meanPileUpEvents ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize Poisson random number generator" ) ;

  release( randSvc ) ;
  return sc;
}

std::string PoissonPileUp::getFilename() {
  return m_filename;
}

unsigned int PoissonPileUp::numberOfPileUp( ) {
  m_currentNumPileUpEvents = m_PoissonDist();
  printPileUpCounters();
  return m_currentNumPileUpEvents;
}

void PoissonPileUp::printPileUpCounters () {
  info() << "Adding " << m_currentNumPileUpEvents << " pileup events." << endmsg;
}
