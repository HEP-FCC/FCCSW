// $Id$
// ============================================================================
#ifndef LHCBKERNEL_IPARTICLEPROPERTYSVC_H
#define LHCBKERNEL_IPARTICLEPROPERTYSVC_H
// ============================================================================
// Include Files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
// ============================================================================
// forward declarations
// ============================================================================
namespace LHCb { class ParticleID       ; }
namespace LHCb { class ParticleProperty ; }
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  /** @class IParticlePropertySvc Kernel/IParticlePropertySvc.h
   *  The abstract interface to Particle Property Service
   *  @author Iain Last
   *  @author G.Corti
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   */
  class GAUDI_API IParticlePropertySvc : virtual public IInterface
  {
    // ========================================================================
  public: // the  public types 
    // ========================================================================
  public:
    // ========================================================================
    /// the actual type of (ordered) container of particle properties 
    typedef std::vector<const LHCb::ParticleProperty*> ParticleProperties ;
    /// the actual type of iterator over the (ordered) container of properties 
    typedef ParticleProperties::const_iterator         iterator           ;
    // ========================================================================
  public:
    // ========================================================================
    /** get the begin-iterator for the container of particle properties
     *  It is assumed that the container is properly ordered
     *  @return begin-iterator for the container of particle properties 
     */  
    virtual iterator begin () const = 0 ;
    /** get the end-iterator for the container of particle properties
     *  It is assumed that the container is properly ordered
     *  @return end-iterator for the container of particle properties 
     */  
    virtual iterator end   () const = 0 ;
    /// get the container size.
    virtual size_t   size  () const = 0 ;
    /** Retrieve an object by name:
     *
     *  @code 
     *  
     *   LHCb::IParticlePropertySvc* svc = ... ;
     *
     *   const std::string& name = ... ;
     * 
     *   const LHCb::ParticleProperty*  pp = svc -> find ( name ) ;
     *
     *  @endcode 
     *  @param name the particle name
     *  @return pointer to particle property object
     */
    virtual const ParticleProperty* find 
    ( const std::string&      name ) const =  0 ;   // find by particle name 
    /** Retrieve an object by LHCb::ParticleID:
     *
     *  @code 
     *  
     *   LHCb::IParticlePropertySvc* svc = ... ;
     *
     *   const LHCb::ParticleID& pid = ... ;
     *
     *   const LHCb::ParticleProperty*  pp = svc -> find ( pid ) ;
     *
     *  @endcode 
     *  @param  pid     the particle's LHCb::ParticleID 
     *  @return pointer to particle property object
     */
    virtual const ParticleProperty* find 
    ( const LHCb::ParticleID& pid  ) const =  0 ; // find by LHCb::ParticleID
    // ========================================================================
  public:
    // ========================================================================
    /** get the properties according to some criteria 
     * 
     *  e.g. get all leptons:
     *
     *  @code
     *
     *  #include "boost/lambda/lambda.hpp"
     *  #include "boost/lambda/bind.hpp"
     *  ...
     *  using namespace boost::lambda ;
     *  ...
     *
     *  const LHCb::IParticlePropertySvc* svc = ... ;  
     *
     *  typedef LHCb::IParticlePropertySvc::ParticleProperties Vector ; 
     * 
     *  // create the output vector:
     *  Vector leptons ;
     *  // use the service 
     *  svc -> get 
     *    ( bind ( &LHCb::ParticleID::isLepton , 
     *             bind ( &LHCb::ParticleProperty::particleID , _1 ) ) ,
     *      std::back_inserter ( lepton ) ) ; // output 
     *
     *  @endcode 
     *  
     *  Essentially it is just a missing <c>std::copy_if</c> STL-algorithm 
     *
     *  @param cut the predicate
     *  @param output the output iterator
     *  @return the updated position of output iterator
     */
    template <class PREDICATE, class OUTPUT>
    OUTPUT get 
    ( const PREDICATE& cut    , 
      OUTPUT           output ) const ;
    // ========================================================================
  public:
    // ========================================================================
    /** make the charge conjugation for the string/decay descriptor 
     * 
     *  @code 
     * 
     *   std::string decay = "B0 -> pi+ pi-" ;
     *  
     *   LHCb::IParticleProeprtySvc* svc = ... ;
     *  
     *   std::string cc = svc -> cc ( decay ) ;
     *
     *  @endcode 
     *
     *  @param decay the decay descriptor
     *  @return the charge conjugation for the decay descriptor
     */  
    virtual std::string cc ( const std::string& decay ) const = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    /// retrieve the unique interface identifier
    static const InterfaceID& interfaceID() ;          // the unique identifier 
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual and protected destructor 
    virtual ~IParticlePropertySvc() ;       // virtual and protected destructor 
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LHCb
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  namespace ParticleProperties
  {
    // ========================================================================
    /** helper utility for mapping of LHCb::ParticleProperty object into 
     *  non-negative integral sequential identifier 
     *  @code
     * 
     *  const LHCb::ParticlePropertySvc* svc = ... ;
     * 
     *  const LHCb::ParticleProperty* pp = ... ;
     *
     *   size_t index = index ( pp , svc ) ;
     *
     *   if ( !index ) 
     *   {
     *     // error here: conversion is not possible 
     *   }
     * 
     *  @endcode 
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
    size_t index 
    ( const LHCb::ParticleProperty*     property , 
      const LHCb::IParticlePropertySvc* service  ) ;
    // ========================================================================
    /** helper utility for mapping of LHCb::ParticleID object into 
     *  non-negative integral sequential identifier 
     *  @code
     * 
     *  const LHCb::ParticlePropertySvc* svc = ... ;
     * 
     *  const LHCb::ParticleID& pid = ... ;
     *
     *  size_t index = index ( pid , svc ) ;
     *
     *  if ( !index ) 
     *   {
     *     // error here: conversion is not possible 
     *   }
     * 
     *  @endcode 
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
    size_t index
    ( const LHCb::ParticleID&           pid      , 
      const LHCb::IParticlePropertySvc* service  ) ;
    // ========================================================================
    /** the inverse mapping of the integer sequential number onto 
     *  LHCb::ParticleID object
     *  @code
     * 
     *  const LHCb::ParticlePropertySvc* svc = ... ;
     * 
     *  const size_t index = ... 
     *
     *  const LHCb::ParticleProperty* pp = particle ( index , svc ) ;
     *
     *  if ( !pp ) 
     *   {
     *     // error here: conversion is not possible 
     *   }
     * 
     *  @endcode 
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
    const LHCb::ParticleProperty* particle
    ( const size_t                      index    , 
      const LHCb::IParticlePropertySvc* service  ) ;
    // ========================================================================
    /** the inverse mapping of the integer sequential number onto 
     *  LHCb::ParticleID object
     *  @code
     * 
     *  const LHCb::ParticlePropertySvc* svc = ... ;
     * 
     *  const size_t index = ... 
     *
     *  const LHCb::ParticleID pid = particleID ( index , svc ) ;
     *
     *  if ( LHCb::ParticleID() == pid ) 
     *   {
     *     // error here: conversion is not possible 
     *   }
     * 
     *  @endcode 
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
    const LHCb::ParticleID particleID
    ( const size_t                      index    , 
      const LHCb::IParticlePropertySvc* service  ) ;
    // ========================================================================
    /** mapping by pythiaID 
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
    const LHCb::ParticleProperty* byPythiaID 
    ( const int                         pythia ,
      const LHCb::IParticlePropertySvc* svc    ) ;
    // ========================================================================
    /** mapping by EvtGen-name 
     *  
     *  @code 
     * 
     *   const std::string& evtGen
     * 
     *   const LHCb::IParticlePropertySvc* svc = ... ;
     *
     *   const LHCb::ParticleProperty* pp = byEvtGenName ( pythiaID , svc ) ;
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
    const LHCb::ParticleProperty* byEvtGenName
    ( const std::string&                evtGen ,
      const LHCb::IParticlePropertySvc* svc    ) ;
    // ========================================================================
    /** get all particle properties which satisfy the certain criteria
     * 
     *  e.g. select all particles with 'invalid' Pythia ID = 0 :
     * 
     *  @code
     *
     *  #include "boost/lambda/lambda.hpp"
     *  #include "boost/lambda/bind.hpp"
     *
     *  typedef LHCb::IParticlePropertySvc::ParticleProperties Vector ; 
     *  using namespace boost::lambda ;
     *
     *  const LHCb::IParticlePropertySvc* svc = ... ;  
     * 
     *  // create the output vector:
     *  Vector output ;
     *  // use the function 
     *  LHCb::ParticleProperties::get 
     *         ( svc->begin() ,  // begin-sequence
     *           svc->end  () ,  // end-sequence
     *           bind ( &LHCb::ParticleProperty::pythiaID , _1 ) == 0 , // predicate 
     *           std::back_inserter( output ) ) ; // output 
     *
     *  @endcode
     *
     *  or select all particles with 'invalid' EvtGen  = "unknown" :
     * 
     *  @code
     *
     *  #include "boost/lambda/lambda.hpp"
     *  #include "boost/lambda/bind.hpp"
     *
     *  typedef LHCb::IParticlePropertySvc::ParticleProperties Vector ; 
     *  using namespace boost::lambda ;
     *
     *  const LHCb::IParticlePropertySvc* svc = ... ;  
     * 
     *  // create the output vector:
     *  Vector output ;
     *  // use the function 
     *  LHCb::ParticleProperties::get 
     *         ( svc->begin() ,  // begin-sequence
     *           svc->end  () ,  // end-sequence
     *           bind ( &LHCb::ParticleProperty::evtGen , _1 ) == "unknown" , // predicate 
     *           std::back_inserter( output ) ) ; // output 
     *
     *  @endcode
     *
     *  select leptons: 
     *
     *  @code
     *
     *  #include "boost/lambda/lambda.hpp"
     *  #include "boost/lambda/bind.hpp"
     *  ...
     *  ...
     *  const LHCb::IParticlePropertySvc* svc = ... ;  
     *
     *  typedef LHCb::IParticlePropertySvc::ParticleProperties Vector ; 
     *  using namespace boost::lambda ;
     * 
     *  // create the output vector:
     *  Vector leptons ;
     *  // use the function 
     *  LHCb::ParticleProperties::get 
     *         ( svc->begin() ,  // begin-sequence
     *           svc->end  () ,  // end-sequence
     *           bind ( &LHCb::ParticleID::isLepton , bind ( &LHCb::ParticleProperty::particleID ,_1 ) ) ,
     *           std::back_inserter ( lepton ) ) ; // output 
     *
     *  @endcode
     *
     *  Essentially it is just a missing <c>std::copy_if</c> STL-algorithm 
     * 
     *  @param first begin-iterator of input sequence of particle properties 
     *  @param last  end-iterator of input sequence of particle properties 
     *  @param cut the predicate
     *  @param output the output iterator
     *  @return the updated position of output iterator
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-08-03     
     */ 
    template <class INPUT, class PREDICATE, class OUTPUT> 
    OUTPUT  get
    ( INPUT            first  , 
      INPUT            last   , 
      const PREDICATE& cut    , 
      OUTPUT           output ) 
    {
      for ( ; first != last ; ++first ) 
      { if ( cut ( *first ) ) { *output = *first ; ++output ; } }
      return output ;
    }
    // ========================================================================
    /** get all particle properties which satisfy the certain criteria
     * 
     *  e.g. select all particles with 'invalid' Pythia ID = 0 :
     * 
     *  @code
     *
     *  #include "boost/lambda/lambda.hpp"
     *  #include "boost/lambda/bind.hpp"
     *
     *  typedef LHCb::IParticlePropertySvc::ParticleProperties Vector ; 
     *  using namespace boost::lambda ;
     *
     *  const LHCb::IParticlePropertySvc* svc = ... ;  
     * 
     *  // create the output vector:
     *  Vector output ;
     *  // use the function 
     *  LHCb::ParticleProperties::get 
     *    ( svc , 
     *      bind ( &LHCb::ParticleProperty::pythiaID , _1 ) == 0 , // predicate 
     *      std::back_inserter( output ) ) ; // output 
     *
     *  @endcode
     *
     *  or select all particles with 'invalid' EvtGen  = "unknown" :
     * 
     *  @code
     *
     *  #include "boost/lambda/lambda.hpp"
     *  #include "boost/lambda/bind.hpp"
     *
     *  typedef LHCb::IParticlePropertySvc::ParticleProperties Vector ; 
     *  using namespace boost::lambda ;
     *
     *  const LHCb::IParticlePropertySvc* svc = ... ;  
     * 
     *  // create the output vector:
     *  Vector output ;
     *  // use the function 
     *  LHCb::ParticleProperties::get 
     *    ( svc , 
     *      bind ( &LHCb::ParticleProperty::evtGen , _1 ) == "unknown" , // predicate 
     *      std::back_inserter( output ) ) ; // output 
     *
     *  @endcode
     *
     *  Select all leptons:
     *
     *  @code
     *
     *  #include "boost/lambda/lambda.hpp"
     *  #include "boost/lambda/bind.hpp"
     *
     *  typedef LHCb::IParticlePropertySvc::ParticleProperties Vector ; 
     *  using namespace boost::lambda ;
     *
     *  const LHCb::IParticlePropertySvc* svc = ... ;  
     * 
     *  // create the output vector:
     *  Vector leptons ;
     *  // use the function 
     *  LHCb::ParticleProperties::get 
     *    ( svc , 
     *      bind ( &LHCb::ParticleID::isLepton , 
     *      bind ( &LHCb::ParticleProperty::particleID , _1 ) ) ,
     *      std::back_inserter ( leptons ) ) ; // output 
     *
     *  @endcode
     *
     *  @param first begin-iterator of input sequence of particle properties 
     *  @param end   end-iterator of input sequence of particle properties 
     *  @param cut the predicate
     *  @param output the output iterator
     *  @return the updated position of output iterator
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-08-03     
     */ 
    template <class PREDICATE, class OUTPUT> 
    OUTPUT  get
    ( const LHCb::IParticlePropertySvc* service , 
      const PREDICATE&                  cut     , 
      OUTPUT                            output  ) 
    {
      if ( 0 == service ) { return output ; }
      return service -> get ( cut , output ) ;
    }
    // ========================================================================    
    /** get all the properties at once 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-08-03     
     */ 
    LHCb::IParticlePropertySvc::ParticleProperties 
    allProperties ( const LHCb::IParticlePropertySvc* service ) ;
    // ========================================================================
  } //                                end of namespace LHCb::ParticleProperties
  // ==========================================================================
  // get the properties according to some criteria 
  // ==========================================================================
  template <class PREDICATE, class OUTPUT>
  OUTPUT 
  IParticlePropertySvc::get 
  ( const PREDICATE& cut    , 
    OUTPUT           output ) const 
  {
    iterator first = this -> begin() ;
    iterator last  = this -> end  () ;
    return LHCb::ParticleProperties::get ( first , last , cut , output ) ;
  }
  // ==========================================================================
} //                                                      end of namespace LHCb 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif
// ============================================================================


