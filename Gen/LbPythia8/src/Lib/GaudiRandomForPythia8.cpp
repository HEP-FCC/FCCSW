// $Id: GaudiRandomForPythia8.cpp,v 1.1.1.1 2007-07-31 17:02:19 robbep Exp $
// Include files

// local
#include "LbPythia8/GaudiRandomForPythia8.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaudiRandomForPythia8
//
// 2007-07-31 : Arthur de Gromard
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaudiRandomForPythia8::GaudiRandomForPythia8( IRndmGenSvc * i , StatusCode &sc ) {
  sc = m_gaudiGenerator.initialize( i , Rndm::Flat( 0 , 1 ) ) ;
}
//=============================================================================
// Destructor
//=============================================================================
GaudiRandomForPythia8::~GaudiRandomForPythia8() {
  m_gaudiGenerator.finalize( ) ;
} 

//=============================================================================
