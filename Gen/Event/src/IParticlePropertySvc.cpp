// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <algorithm>
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Event/ParticleID.h"
#include "Event/ParticleProperty.h"
#include "Event/IParticlePropertySvc.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
// ============================================================================
/** @file
 *  Implementation file for class LHCb::IParticlePropertySvc
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-08-03 
 */
// ============================================================================
// retrieve the unique interface identrifier
// ============================================================================
const InterfaceID& LHCb::IParticlePropertySvc::interfaceID()
{
  // the unique identifier
  static const InterfaceID s_ID ( "LHCb::IParticlePropertySvc" , 1 , 0 );
  return s_ID ;
}
// ============================================================================
// virtual and protected destructor
// ============================================================================
LHCb::IParticlePropertySvc::~IParticlePropertySvc() {} // protected destructor
// ============================================================================
/* helper utility for mapping of LHCb::ParticleProperty object into 
 *  non-negative integral sequential identifier 
 *
 *  This appears to be useful operation, but since it is 
 *  "pure technical" it does not appear as interface method.
 *
 *  For invalid/missing property and/or  service 
 *  <c>0</c> is returned. The valid result is always 
 *  satisfy the condition: <c> index <= service->size() </c>
 *  
 *  @param property the property to be mapped 
 *  @param service the service 
 *  @return the sequential non-negative index
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-08-03
 */
// ============================================================================
size_t LHCb::ParticleProperties::index
( const LHCb::ParticleProperty*     property ,
  const LHCb::IParticlePropertySvc* service  )
{
  if ( 0 == property || 0 == service ) { return 0 ; }               // RETURN
  // ==========================================================================
  LHCb::IParticlePropertySvc::iterator first = service -> begin () ;
  LHCb::IParticlePropertySvc::iterator last  = service -> end   () ;
  // start the binary_search
  static const LHCb::ParticleProperty::Compare cmp = LHCb::ParticleProperty::Compare() ;
  LHCb::IParticlePropertySvc::iterator ifind =
    std::lower_bound ( first , last , property , cmp ) ;
  return
    last != ifind && !cmp ( *ifind, property ) ?  (ifind-first+1) : 0 ;
}
// ============================================================================
/* helper utility for mapping of LHCb::ParticleID object into 
 *  non-negative integral sequential identifier 
 *
 *  This appears to be useful operation, but since it is 
 *  "pure technical" it does not appear as interface method.
 *
 *  For invalid/missing PID and/or  service 
 *  <c>0</c> is returned. The valid result is always 
 *  satisfy the condition: <c> index <= service->size() </c>
 *  
 *  @param pid the object to be mapped 
 *  @param service the service 
 *  @return the sequential non-negative index 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-08-03
 */
// ============================================================================
size_t LHCb::ParticleProperties::index
( const LHCb::ParticleID&           pid      ,
  const LHCb::IParticlePropertySvc* service  )
{
  if ( 0 == service ) { return 0 ; }               // RETURN
  // ==========================================================================
  const LHCb::ParticleProperty* pp = service->find ( pid ) ;
  return 0 == pp ? 0 : LHCb::ParticleProperties::index ( pp , service ) ;
}
// ============================================================================
/* the inverse mapping of the integer sequential number onto 
 *  LHCb::ParticleID object
 *
 *  This appears to be useful operation, but since it is 
 *  "pure technical" it does not appear as interface method.
 *
 *  For invalid/missing PID and/or  service 
 *  <c>NULL</c> is returned.
 *  
 *  @param pid the object to be mapped 
 *  @param service the service 
 *  @return the sequential non-negative index 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-08-03
 */
// ============================================================================
const LHCb::ParticleProperty*
LHCb::ParticleProperties::particle
( const size_t                      index    ,
  const LHCb::IParticlePropertySvc* service  )
{
  if (  0 == index || 0 == service ) { return 0 ; }                 // RETURN
  // get the iterators from the service
  LHCb::IParticlePropertySvc::iterator first = service -> begin () ;
  LHCb::IParticlePropertySvc::iterator last  = service -> end   () ;
  if ( index > (size_t) std::distance ( first , last ) ) { return 0 ; } // RETURN
  std::advance ( first , index - 1 ) ;
  return *first ;                                                   // RETURN
}
// ============================================================================
/*  the inverse mapping of the integer sequential number onto 
 *  LHCb::ParticleID object
 *
 *  This appears to be useful operation, but since it is 
 *  "pure technical" it does not appear as interface method.
 *
 *  For invalid/missing index and/or  service 
 *  <c>LHCb::ParticleID()</c> is returned. 
 *  
 *  @param pid the object to be mapped
 *  @param service the service
 *  @return the sequential non-negative index 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-08-03
 */
// ============================================================================
const LHCb::ParticleID LHCb::ParticleProperties::particleID
( const size_t                      index    ,
  const LHCb::IParticlePropertySvc* service  )
{
  if ( 0 == index || 0 == service ) { return LHCb::ParticleID() ; }   // RETURN
  const LHCb::ParticleProperty* pp =
    LHCb::ParticleProperties::particle ( index , service ) ;
  if ( 0 == pp ) { return LHCb::ParticleID() ; }
  return pp->particleID () ;
}
// ============================================================================
/* mapping by pythiaID 
 *  
 *  @code 
 * 
 *   const int pythiaID = ... ;
 * 
 *   const LHCb::IParticlePropertySvc* svc = ... ;
 *
 *   const LHCb::ParticleProeprty* pp = byPythiaID( pythiaID , svc ) ;
 *
 *  @endcode
 *
 *  @attention the method is not very efficient and should not be abused
 *  @see LHCb::ParticleProperties::particle
 *  @param pythia pythia identifier
 *  @param svc    pointer to particle property service
 *  @return the particle property for the given pythia ID
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-08-03
 */
// ============================================================================
const LHCb::ParticleProperty*
LHCb::ParticleProperties::byPythiaID
( const int                         pythia ,
  const LHCb::IParticlePropertySvc* svc    )
{
  if ( 0 == svc ) { return 0 ; }
  // to be efficient
  // 1) try to use PDG-ID (fast, logarithmic search)
  const LHCb::ParticleProperty* pp =  svc->find ( LHCb::ParticleID ( pythia ) ) ;
  // 2) check the proper pythia ID
  if ( 0 != pp && pythia == pp -> pythiaID () ) { return pp ; }       // RETURN
  // 3) use thr resular (linear search)
  const LHCb::IParticlePropertySvc::iterator begin = svc -> begin () ;
  const LHCb::IParticlePropertySvc::iterator end   = svc -> end   () ;
  LHCb::IParticlePropertySvc::iterator found =
    std::find_if (  begin , end ,
                    boost::lambda::bind ( &LHCb::ParticleProperty::pythiaID  ,
                                          boost::lambda::_1 ) == pythia ) ;
  //
  if ( end == found  ) { return 0 ; }
  //
  return *found ;
}
// ============================================================================
/*  mapping by EvtGen-name 
 * 
 *  @code
 *
 *   const std::string& evtGen = ...
 * 
 *   const LHCb::IParticlePropertySvc* svc = ... ;
 *
 *   const LHCb::ParticleProperty* pp = byEvtGenName ( evtGen , svc ) ;
 *
 *  @endcode 
 *
 *  @attention the method is not very efficient and should not be abused 
 *  @see LHCb::ParticleProperties::particle
 *  @param evtGen the particle naem in EvtGen-generator 
 *  @param svc    pointer to particle property service
 *  @return the particle property for the given EvtGen-name
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-08-03
 */
// ============================================================================
const LHCb::ParticleProperty*
LHCb::ParticleProperties::byEvtGenName
( const std::string&                evtGen ,
  const LHCb::IParticlePropertySvc* svc    )
{
  if ( 0 == svc ) { return 0 ; }
  // to be more efficient:
  // 1) try to use the regualr name (fast, logarithmic search)
  const LHCb::ParticleProperty* pp =  svc->find ( evtGen ) ;
  // 2) check the proper evtgen name
  if ( 0 != pp && evtGen == pp->evtGen() ) { return pp ; }       // RETURN
  // 3) use the resular (linear search)
  const LHCb::IParticlePropertySvc::iterator begin = svc -> begin () ;
  const LHCb::IParticlePropertySvc::iterator end   = svc -> end   () ;
  LHCb::IParticlePropertySvc::iterator found =
    std::find_if (  begin , end ,
                    boost::lambda::bind ( &LHCb::ParticleProperty::evtGen ,
                                          boost::lambda::_1 ) == evtGen ) ;
  //
  if ( end == found  ) { return 0 ; }
  //
  return *found ;
}
// ============================================================================
/*  get all the properties at once
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-08-03   
 */
// ============================================================================
LHCb::IParticlePropertySvc::ParticleProperties
LHCb::ParticleProperties::allProperties
( const LHCb::IParticlePropertySvc* service )
{
  return ( 0 != service ) ?
    LHCb::IParticlePropertySvc::ParticleProperties
    ( service->begin () , service->end() ) :
    LHCb::IParticlePropertySvc::ParticleProperties () ;
}
// ============================================================================
// The END
// ============================================================================
