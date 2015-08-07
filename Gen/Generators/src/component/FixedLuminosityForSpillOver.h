// $Id: FixedLuminosityForSpillOver.h,v 1.2 2009-04-07 16:11:21 gcorti Exp $
#ifndef GENERATORS_FIXEDLUMINOSITYFORSPILLOVER_H 
#define GENERATORS_FIXEDLUMINOSITYFORSPILLOVER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IPileUpTool.h"

// forward declaration
class IRndmGenSvc ;
class ICounterLogFile ;

/** @class FixedLuminosityForSpillOver FixedLuminosityForSpillOver.h "FixedLuminosityForSpillOver.h"
 *  
 *  Tool to compute variable number of pile up events
 *  depending on beam parameters.
 *  To be used for spill over generation because it accepts events
 *  with 0 interaction
 * 
 *  @author Patrick Robbe
 *  @date   2009-03-30
 */
class FixedLuminosityForSpillOver : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  FixedLuminosityForSpillOver( const std::string& type, const std::string& name,
		               const IInterface* parent) ;

  virtual ~FixedLuminosityForSpillOver( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /// Finalize method
  virtual StatusCode finalize( ) ;

  /** Implements IPileUpTool::numberOfPileUp
   *  Returns the number of pile-up interactions in one event. It follows
   *  a Poisson distribution with 
   *  mean = Luminosity * cross_section / crossing_rate.
   *  The fixed luminosity is returned as the currentLuminosity.
   */
  virtual unsigned int numberOfPileUp( ) ;

  /// Implements IPileUpTool::printPileUpCounters
  virtual void printPileUpCounters( ) ;

protected:

private:
  ICounterLogFile * m_xmlLogTool ; ///< Log file in XML
  
  std::string m_beamParameters ; ///< Location of beam parameters (set by options)

  int    m_numberOfZeroInteraction ; ///< Counter of empty events

  int    m_nEvents ; ///< Counter of events (including empty events)

  IRndmGenSvc * m_randSvc ; ///< Pointer to random number generator service
};
#endif // GENERATORS_FIXEDLUMINOSITYFORSPILLOVER_H
