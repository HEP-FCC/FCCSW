#ifndef GENERATION_POISSONPILEUP_H 
#define GENERATION_POISSONPILEUP_H

#include "Generation/IPileUpTool.h"

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
  PoissonPileUp( const std::string& type , const std::string& name,
                    const IInterface* parent ) ;
  
  virtual ~PoissonPileUp();

  virtual StatusCode initialize( ) ;
  
  virtual unsigned int numberOfPileUp( ) ;

  virtual double getMeanPileUp();

  virtual void printPileUpCounters( ) ;
  
  virtual std::string getFilename();
  
 private:
  /// average number of min bias events to pile on signal event.
  double m_meanPileUpEvents;
  /// holds last realization of the random variable
  unsigned int m_currentNumPileUpEvents;
  std::string m_filename;

  Rndm::Numbers m_PoissonDist ; ///< Poisson random number generator
};

#endif // GENERATION_POISSONPILEUP_H
