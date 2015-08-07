// $Id: EvtReport.cpp,v 1.3 2007-10-10 20:07:24 robbep Exp $
// Overwrite EvtGen output messages

#include "Generators/StreamForGenerator.h"
#include "Event/EvtReport.hh"

std::ostringstream dummyStr ;

//=============================================================================
// Reimplementation of print facility of EvtGen
//=============================================================================
std::ostream & report( Severity severity , const char * facility )
{
  dummyStr.str("") ;
  if ( severity < WARNING ) {
    if ( 0 != facility[0] ) {
      (*StreamForGenerator::getStream()) << MSG::ERROR << facility << " Error from EvtGen"
                      << endmsg ;
    }
    else (*StreamForGenerator::getStream()) << MSG::ERROR << "Error from EvtGen"
                         << endmsg ;
  } else if ( severity < INFO ) {
    if ( 0 != facility[0] ) {
      (*StreamForGenerator::getStream()) << MSG::INFO ;
      if ( StreamForGenerator::getStream()->isActive() ) std::cout << facility << ":" ;
    } else (*StreamForGenerator::getStream()) << MSG::INFO ;
  } else {
    if ( 0 != facility[0] ) {
      (*StreamForGenerator::getStream()) << MSG::DEBUG ;
      if ( StreamForGenerator::getStream()->isActive() ) std::cout << facility << ":" ;
    } else ( *StreamForGenerator::getStream() ) << MSG::DEBUG ;
  }
  if ( StreamForGenerator::getStream()->isActive() ) return std::cout ;
  else return dummyStr ;
}
