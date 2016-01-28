
#include "ConstPileUp.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( ConstPileUp )

ConstPileUp::ConstPileUp( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
    : GaudiTool ( type, name , parent ) {
  declareInterface< IPileUpTool >( this );
  
  declareProperty( "numPileUpEvents", m_numPileUpEvents = 0 );
  declareProperty( "Filename_Pileup", m_filename = "" );
}

ConstPileUp::~ConstPileUp( ) { ; }

StatusCode ConstPileUp::initialize( ) {
  StatusCode sc = GaudiTool::initialize( );
  return sc;
}

unsigned int ConstPileUp::numberOfPileUp( ) {
  printPileUpCounters();
  return m_numPileUpEvents;
}

std::string ConstPileUp::getFilename() {
  return m_filename;
}

void ConstPileUp::printPileUpCounters () {
  info() << "Adding " << m_numPileUpEvents << " pileup events." << endmsg;
}
