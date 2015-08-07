// $Id: StreamForGenerator.cpp,v 1.2 2007-10-11 13:23:27 robbep Exp $
// Include files

// local
#include "Generators/StreamForGenerator.h"

MsgStream * StreamForGenerator::s_gaudiStream = 0 ;

MsgStream *& StreamForGenerator::getStream() {
  return s_gaudiStream ;
}
