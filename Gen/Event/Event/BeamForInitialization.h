// $Id: $
#ifndef GENEVENT_BEAMFORINITIALIZATION_H 
#define GENEVENT_BEAMFORINITIALIZATION_H 1

namespace LHCb {
  class BeamParameters ;
}

/** @class BeamForInitialization BeamForInitialization.h GenEvent/BeamForInitialization.h
 *  Simple class to hold beam parameters for initialization of generators
 *
 *  @author Patrick Robbe
 *  @data   2011-05-27
 */
class BeamForInitialization {
public:
  static LHCb::BeamParameters *& getInitialBeamParameters( ) ;
    
protected:

private:
  static LHCb::BeamParameters * s_initialBeamParameters ;
} ;
#endif

