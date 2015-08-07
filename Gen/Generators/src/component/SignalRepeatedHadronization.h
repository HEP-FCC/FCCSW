// $Id: SignalRepeatedHadronization.h,v 1.6 2005-12-31 17:33:12 robbep Exp $
#ifndef GENERATORS_SIGNALREPEATEDHADRONIZATION_H 
#define GENERATORS_SIGNALREPEATEDHADRONIZATION_H 1

// Include files
#include "Generators/Signal.h" 

/** @class SignalRepeatedHadronization SignalRepeatedHadronization.h "SignalRepeatedHadronization.h"
 *  
 *  Tool for signal generation with repeated hadronization method.
 *  Concrete implementation of ISampleGenerationTool using the
 *  Signal base class.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class SignalRepeatedHadronization : public Signal {
public:
  /// Standard constructor
  SignalRepeatedHadronization( const std::string& type, 
                               const std::string& name,
                               const IInterface* parent);
  
  virtual ~SignalRepeatedHadronization( ); ///< Destructor

  virtual StatusCode initialize( ) ; ///< Initialize
  
  /** Generate events with at least one interaction containing the signal
   *  Implements ISampleGenerationTool::generate.
   *  -# Turn off hadronization process (using 
   *     IProductionTool::turnOffFragmentation)
   *  -# Generate interaction with IProductionTool
   *  -# If event has a b quark, save parton event (using
   *     IProductionTool::savePartonEvent
   *  -# In all cases, hadronize the event (using 
   *     IProductionTool::hadronize.
   *  -# If interaction has b quark, recall (using 
   *     IProductionTool::retrievePartonEvent) and 
   *     hadronize this event (with IProductionTool:hadronize)
   *     until it contains a signal B.
   *  -# Apply generator level cut and keep or reject full
   *     event (if rejected, the interaction is not hadronized
   *     again and is not kept).
   */
  virtual bool generate( const unsigned int nPileUp , 
                         LHCb::HepMCEvents * theEvents ,
                         LHCb::GenCollisions * theCollisions ) ;

protected:
  /// Delete full content of an event
  void Clear( HepMC::GenEvent * theEvent ) const ;
  
private:
  /// Maximum number of allowed repetitions of hadronization (set by options)
  unsigned int  m_maxNumberOfRepetitions ;

  /// List of quarks of signal particles to generate
  PIDs          m_pidQuarks        ;
};
#endif // GENERATORS_SIGNALREPEATEDHADRONIZATION_H
