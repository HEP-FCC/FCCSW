///////////////////////////////////////////////////////////////////
// Charged.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_CHARGED_H
#define TRKPARAMETERSBASE_CHARGED_H

#include "TrkParametersBase/ChargeDefinition.h"
#include "GaudiKernel/MsgStream.h"

namespace Trk {

/** @class Charged

   Charge definition class : charged
   for templating the (Charged)TrackParameters
   
   @author Andreas.Salzburger@cern.ch   
*/
class Charged : public ChargeDefinition
{
  public:
   /** default ctor for POOL*/
   Charged() :
     ChargeDefinition(1.)
     {}
    
    /** Copy constructor */
    Charged( const Charged& chdef) :
     ChargeDefinition(chdef)
     {}
    
    /** Destructor */
    virtual ~Charged(){}
    
    /**Dumps relevant information about the track parameters into the ostream.*/
    void dump( MsgStream& out ) const;
    void dump( std::ostream& out ) const;

 };

inline void Charged::dump( MsgStream& out) const
{ out << "Trk::Charged - charge value : " << double(*this); }

inline void Charged::dump( std::ostream& out) const
{ out << "Trk::Charged - charge value : " << double(*this); }


} //end ns

#endif
