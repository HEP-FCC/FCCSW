///////////////////////////////////////////////////////////////////
// Neutral.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_NEUTRAL_H
#define TRKEVENTPRIMITIVES_NEUTRAL_H

#include "TrkParametersBase/ChargeDefinition.h"
#include "GaudiKernel/MsgStream.h"

namespace Trk {

/** @class Neutral

   Charge definition class : charged
   for templating the NeutralTrackParameters
   
   @author Andreas.Salzburger@cern.ch   
*/
class Neutral : public ChargeDefinition
{
  public:
    /** default ctor for POOL*/
    Neutral() :
     ChargeDefinition()
     {}

    /** Copy constructor */
    Neutral( const Neutral& chdef) :
     ChargeDefinition(chdef)
     {}

    /** Destructor */
    virtual ~Neutral(){}
    
    /**Dumps relevant information about the track parameters into the ostream.*/
    void dump( MsgStream& out ) const;
    void dump( std::ostream& out ) const;

 };

inline void Neutral::dump( MsgStream& out) const
{ out << "Trk::Neutral";  }

inline void Neutral::dump( std::ostream& out) const
{ out << "Trk::Neutral";  }


} //end ns

#endif
