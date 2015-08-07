// $Id: SignalForcedFragmentation.h,v 1.6 2008-05-29 14:22:00 gcorti Exp $
#ifndef GENERATORS_SIGNALFORCEDFRAGMENTATION_H 
#define GENERATORS_SIGNALFORCEDFRAGMENTATION_H 1

// Include files
#include "Generators/Signal.h"
#include "GaudiKernel/Transform4DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"

/** @class SignalForcedFragmentation SignalForcedFragmentation.h "SignalForcedFragmentation.h"
 *  
 *  Tool for signal generation with forced fragmentation.
 *  Concrete implementation of ISampleGenerationTool using
 *  the Signal base class.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class SignalForcedFragmentation : public Signal {
 public:
  /// Standard constructor
  SignalForcedFragmentation( const std::string & type , 
                             const std::string & name ,
                             const IInterface * parent ) ;
  
  virtual ~SignalForcedFragmentation( ); ///< Destructor

  virtual StatusCode initialize( ) ; ///< Initialize
  
  /** Generate set of interactions.
   *  Implements ISampleGenerationTool::generate
   *  -# Choose randomly a flavour to generate for each event
   *  -# Generate with the IDecayTool the signal event (at rest)
   *     of this flavour.
   *  -# Force the fragmentation into this flavour when an 
   *     interaction contains a b quark, calling 
   *     IProductionTool::setupForcedFragmentation.
   */
  virtual bool generate( const unsigned int nPileUp , 
                         LHCb::HepMCEvents * theEvents ,
                         LHCb::GenCollisions * theCollisions ) ;
 private:
  /** Boost a particle at rest in the lab frame.
   *  @param[in,out] theSignal       Particle in the lab. frame 
   *                                 to boost to
   *  @param[in]     theSignalAtRest Decay tree at rest to boost
   *  @param[in]     theVector       3-momentum boost vector
   */
  StatusCode boostTree( HepMC::GenParticle * theSignal , 
                        const HepMC::GenParticle * theSignalAtRest ,
                        const ROOT::Math::Boost & theBoost ) const ;

  double m_signalMass ; ///< Mass of the signal particle
};


#endif // GENERATORS_SIGNALFORCEDFRAGMENTATION_H
