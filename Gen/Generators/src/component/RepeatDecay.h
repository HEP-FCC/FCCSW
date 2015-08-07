// $Id: RepeatDecay.h,v 1.1 2007-04-01 21:28:12 robbep Exp $
#ifndef GENERATORS_REPEATDECAY_H 
#define GENERATORS_REPEATDECAY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/ISampleGenerationTool.h" 

// forward declarations

namespace HepMC {
  class GenParticle ;
}

/** @class RepeatDecay RepeatDecay.h "RepeatDecay.h" 
 *  
 *  Tool for the generation of samples re-using the same event
 *  several times and re-decaying it only. When an event has been found
 *  passing the generator level cut, it is kept and re-decayed several times
 *  (applying again the same cut on it).
 *  Concrete implementation of a ISampleGenerationTool .
 *
 *  @author Patrick Robbe
 *  @date   2007-03-31
 */
class RepeatDecay : public GaudiTool , 
                    virtual public ISampleGenerationTool  {
 public:
  /// Standard constructor
  RepeatDecay( const std::string& type, const std::string& name,
             const IInterface* parent);
  
  virtual ~RepeatDecay( ); ///< Destructor

  /** Initialize method 
   */
  virtual StatusCode initialize( ) ;

  /** Generate a set of interactions.
   *  Implements ISampleGenerationTool::generate.
   *  -# Generate a set of interactions with the ISampleGenerationTool,
   *       if there is no event in memory or the same event has been re-used
   *       too many times.
   *  -# If there is an event in memory, just erase the particles in it.
   */
  virtual bool generate( const unsigned int nPileUp ,
                         LHCb::HepMCEvents * theEvents ,
                         LHCb::GenCollisions * theCollisions ) ;

  /** Print generation counters.
   *  Implements ISampleGenerationTool::printCounters.
   */
  virtual void printCounters( ) const ;

 protected:

 private:
  
  /// Name of the tool used to generate the events (set by option)
  std::string m_baseToolName ; 

  /// Tool used to generate the base events which are re-decayed
  ISampleGenerationTool * m_baseTool ;

  /// Number of times to redecay the events (set by option)
  unsigned int m_nRedecayLimit ;

  /// counter of repetitions
  unsigned int m_nRedecay ;

  /// Memorized events
  LHCb::HepMCEvents m_theMemorizedEvents ;
  
  /// Memorized collisions
  LHCb::GenCollisions m_theMemorizedCollisions ;
  
  /// Copy a set of events into another
  void copyEvents( LHCb::HepMCEvents * from , 
                   LHCb::HepMCEvents * to )  ;
  
  /* Copy a set of collisions into another, with the list of corresponding 
   * events
   */
  void copyCollisions( LHCb::GenCollisions * from , LHCb::GenCollisions * to ,
                       LHCb::HepMCEvents * theEvents ) ;
};
#endif // GENERATORS_REPEATDECAY_H
