// $Id: $
// local
#include "Event/BeamForInitialization.h"

// Include files
#include "Event/BeamParameters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BeamForInitialization
//
// 2011-05-27 : Patrick Robbe
//-----------------------------------------------------------------------------

LHCb::BeamParameters * BeamForInitialization::s_initialBeamParameters = 0 ;

LHCb::BeamParameters *& BeamForInitialization::getInitialBeamParameters( ) {
  return s_initialBeamParameters ;
}
