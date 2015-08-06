///////////////////////////////////////////////////////////////////
// ChargeDefinition.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_CHARGEDEFINITION_H
#define TRKPARAMETERSBASE_CHARGEDEFINITION_H

#include "GaudiKernel/MsgStream.h"
#include <iostream>

namespace Trk {


/** @class ChargeDefinition

   Charge definition base class for
   templating the TrackParameters
   
   @author Andreas.Salzburger@cern.ch   
*/
class ChargeDefinition
{
	public:
    /** default ctor for POOL*/
    ChargeDefinition() :
     m_charge(0.)
     {}

    /** ctor with argument */
    ChargeDefinition(double charge) :
     m_charge(charge)
     {}

	/** Copy constructor */
	ChargeDefinition( const ChargeDefinition& chdef) :
     m_charge(chdef.m_charge)
     {}

	/** Destructor */
	virtual ~ChargeDefinition(){}

	/** Return value */
	virtual double value() const { return m_charge; }

    /** take it as a double  */
    operator double () const { return m_charge; }    

    void flipSign() const { m_charge *= -1.;}
    
    /**Dumps relevant information about the track parameters into the ostream.*/
    virtual void dump( MsgStream& out ) const = 0;
    virtual void dump( std::ostream& out ) const = 0;

   private:
  	mutable double m_charge;           //!< the charge
 };

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const ChargeDefinition& chdef);
std::ostream& operator << ( std::ostream& sl, const ChargeDefinition& chdef); 

} //end ns

#endif // TRKEVENTPRIMITIVES_FITQUALITY_H
