// $Id: VariableLuminosity.h,v 1.3 2009-04-07 16:11:21 gcorti Exp $
#ifndef GENERATORS_VARIABLELUMINOSITY_H 
#define GENERATORS_VARIABLELUMINOSITY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IPileUpTool.h"

// Forward declarations
class IRndmGenSvc ;
class ICounterLogFile ;

/** @class VariableLuminosity VariableLuminosity.h "VariableLuminosity.h"
 *  
 *  Tool to compute variable number of pile up events depending on beam 
 *  parameters, with time-dependant luminosity. Concrete implementation
 *  of a IPileUpTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */
class VariableLuminosity : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  VariableLuminosity( const std::string& type, const std::string& name,
                      const IInterface* parent);

  virtual ~VariableLuminosity( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /// Finalize method
  virtual StatusCode finalize( ) ;

  /** Compute number of interactions and returns luminosity
   *  Implements IPileUpTool::numberOfPileUp.
   *  The number of pileup interactions follows a Poisson law
   *  with mean equal to Luminosity * cross_section / crossing_frequency
   *  The Luminosity is exponentially decreasing with beam decay time.
   *  The mean luminosity is given in options so the maximum luminosity 
   *  (at t=0) is computed using the fill duration.
   */
  virtual unsigned int numberOfPileUp( ) ;

  /// Implements IPileUpTool::printPileUpCounters
  virtual void printPileUpCounters( ) ;

private:
  std::string m_beamParameters ; ///< Location of beam parameters (set by options)

  double m_fillDuration ; ///< Fill duration (set by options)

  double m_beamDecayTime ; ///< Beam decay time (set by options)

  /// XML Log tool
  ICounterLogFile * m_xmlLogTool ;

  /// Counter of empty interactions
  int    m_numberOfZeroInteraction ;

  /// Counter of events (including empty interactions)
  int    m_nEvents ;

  /// Random number generator service
  IRndmGenSvc * m_randSvc ;

  /// Flat random number generator
  Rndm::Numbers m_flatGenerator ;
};
#endif // GENERATORS_VARIABLELUMINOSITY_H
