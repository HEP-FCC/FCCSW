// $Id: RandomForGenerator.cpp,v 1.2 2007-10-11 13:23:26 robbep Exp $
// Include files

// local
#include "Generators/RandomForGenerator.h"

Rndm::Numbers RandomForGenerator::s_randgaudi ;

Rndm::Numbers & RandomForGenerator::getNumbers() {
  return s_randgaudi ;
}

double RandomForGenerator::flat( ) {
  return s_randgaudi() ;
}

