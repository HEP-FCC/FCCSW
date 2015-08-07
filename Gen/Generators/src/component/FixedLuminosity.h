// $Id: FixedLuminosity.h,v 1.4 2009-04-07 16:11:21 gcorti Exp $
#ifndef GENERATORS_FIXEDLUMINOSITY_H 
#define GENERATORS_FIXEDLUMINOSITY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IPileUpTool.h"

// forward declaration
class IRndmGenSvc ;
class ICounterLogFile ;

/** @class FixedLuminosity FixedLuminosity.h "FixedLuminosity.h"
 *  
 *  Tool to compute variable number of pile up events
 *  depending on beam parameters
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */
class FixedLuminosity : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  FixedLuminosity( const std::string& type, const std::string& name,
                   const IInterface* parent) ;

  virtual ~FixedLuminosity( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /// Finalize method
  virtual StatusCode finalize( ) ;

  /** Implements IPileUpTool::numberOfPileUp
   *  Returns the number of pile-up interactions in one event. It follows
   *  a Poisson distribution with 
   *  mean = Luminosity * cross_section / crossing_rate.
   *  The fixed luminosity is returned in the GenHeader.
   */
  virtual unsigned int numberOfPileUp( ) ;

  /// Implements IPileUpTool::printPileUpCounters
  virtual void printPileUpCounters( ) ;

protected:

private:
  ICounterLogFile * m_xmlLogTool ; ///< XML File for generator counters
  
  std::string m_beamParameters ; ///< Location of beam parameters (set by options)

  int    m_numberOfZeroInteraction ; ///< Counter of empty events

  int    m_nEvents ; ///< Counter of events (including empty events)

  IRndmGenSvc * m_randSvc ; ///< Pointer to random number generator service
};
#endif // GENERATORS_FIXEDLUMINOSITY_H
