// $Id: FlatNParticles.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_FLATNPARTICLES_H 
#define PARTICLEGUNS_FLATNPARTICLES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IPileUpTool.h"

// forward declaration
class IRndmGenSvc ;

/** @class FlatNParticles FlatNParticles.h "FlatNParticles.h"
 *  
 *  Tool to generate random with flat distribution a number of 
 *  particles, to be used with ParticleGun algorithm
 * 
 *  @author Patrick Robbe
 *  @date   2008-05-18
 */
class FlatNParticles : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  FlatNParticles( const std::string& type, const std::string& name,
                   const IInterface* parent) ;

  virtual ~FlatNParticles( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /** Implements IPileUpTool::numberOfPileUp
   *  Returns the number of particles in one event. It follows
   *  a flat distribution 
   */
  virtual unsigned int numberOfPileUp( ) ;

  /// Implements IPileUpTool::printPileUpCounters
  virtual void printPileUpCounters( ) { ; } ; 
  
protected:

private:
  unsigned int m_minNumberOfParticles ; ///< Minimum number of particles (set by options)
  unsigned int m_maxNumberOfParticles ; ///< Maximum number of particles (set by options)
  
  Rndm::Numbers m_flatGenerator ; ///< Flat random number generator
};
#endif // PARTICLEGUNS_FLATNPARTICLES_H
