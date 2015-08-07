// $Id: Special.h,v 1.5 2007-03-08 13:42:17 robbep Exp $
#ifndef GENERATORS_SPECIAL_H 
#define GENERATORS_SPECIAL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/ExternalGenerator.h" 

// forward declarations
class ICounterLogFile ;

/** @class Special Special.h 
 *  
 *  Tool for special samples (Higgs, W, Z, ...) generation.
 *  Concrete implementation of ISampleGenerationTool using
 *  the ExternalGenerator base class.
 *
 *  @author Patrick Robbe
 *  @date   2005-11-14
 */
class Special : public ExternalGenerator {
 public:
  /// Standard constructor
  Special( const std::string& type, const std::string& name,
           const IInterface* parent);
  
  virtual ~Special( ); ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize( ) ;

  /// Finalize function
  virtual StatusCode finalize() ;
  
  /** Generate a single interaction (No Pile-up for the moment.
   *  Implements ISampleGenerationTool::generate.
   *  Accepts all events generated with the IProductionTool
   *  (usually configured with special options) and passing
   *  the generator level cut.
   */
  virtual bool generate( const unsigned int nPileUp , 
                         LHCb::HepMCEvents * theEvents ,
                         LHCb::GenCollisions * theCollisions ) ;

  /// Implements ISampleGenerationTool::printCounters
  virtual void printCounters( ) const ;

private:
  /// XML Log file
  ICounterLogFile * m_xmlLogTool ;
  
  /// Counter of events before the generator level cut
  unsigned int m_nEventsBeforeCut ;

  /// Counter of events after the generator level cut
  unsigned int m_nEventsAfterCut ;

  /// Number of pile-up events to generate at once
  unsigned int m_maxInteractions ;

  /// Vector to contain pile-up events
  std::vector< HepMC::GenEvent * > m_pileUpEventsVector ;
  
  /// Vector to contain collision infos
  std::vector< LHCb::GenCollision * > m_pileUpCollisionsVector ;
  
  /// function to generate a set of pile up events
  void generatePileUp() ;
  
  /// production tool which generates pile-up interactions
  IProductionTool * m_pileUpProductionTool ;

  /// flag to reinitialize the pile up generator
  bool m_reinitialize ;

  /// Name of the production tool for pile-up
  std::string m_pileUpProductionToolName ;

  /// Copy collision FROM to TO
  void copyCollision( const LHCb::GenCollision * FROM , 
                      LHCb::GenCollision * TO ) const ;
};
#endif // GENERATORS_SPECIAL_H
