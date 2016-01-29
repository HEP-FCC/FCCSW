#ifndef GENERATION_POISSONPILEUP_H 
#define GENERATION_POISSONPILEUP_H 1

#include "IPileUpTool.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 


/** @class PoissonPileUp PoissonPileUp.h "ConstPileup.h"
 *  
 *  Tool to generate number of pile-up events to be mixed with signal event.
 *  Concrete implementation of a IPileUpTool, returning a random variable
 *   drawn from a Poisson distribution centered around the average number
 *   of pileup events.
 * 
 *  @author Valentin Volkl
 *  @date   2016-01-18
 */

class PoissonPileUp : public GaudiTool, virtual public IPileUpTool {
 public:
  /// Standard constructor
  PoissonPileUp( const std::string& type , const std::string& name,
                    const IInterface* parent ) ;
  
  virtual ~PoissonPileUp( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /** Implements IPileUpTool::numberOfPileUp.
   */
  virtual unsigned int numberOfPileUp( ) ;
  /** Implements IPileUpTool::printPileUpCounters.
   */
  virtual void printPileUpCounters( ) ;
  
  virtual std::string getFilename();
  
 private:
  /// average number of min bias events to pile on signal event.
  double m_meanPileUpEvents;
  unsigned int m_currentNumPileUpEvents;
  std::string m_filename;


  Rndm::Numbers m_PoissonDist ; ///< Poisson random number generator
};

#endif // GENERATION_POISSONPILEUP_H
