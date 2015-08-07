// $Id: MinimumBias.h,v 1.3 2005-12-31 17:33:12 robbep Exp $
#ifndef GENERATORS_MINIMUMBIAS_H 
#define GENERATORS_MINIMUMBIAS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/ExternalGenerator.h" 

/** @class MinimumBias MinimumBias.h "MinimumBias.h"
 *  
 *  Tool for minimum bias generation. Concrete implementation 
 *  of ISampleGenerationTool using the base class ExternalGenerator.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class MinimumBias : public ExternalGenerator {
 public:
  /// Standard constructor
  MinimumBias( const std::string & type , const std::string & name ,
               const IInterface * parent ) ;

  virtual ~MinimumBias( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;

  /** Generate minimum bias interactions without any cut.
   *  Implements ISampleGeneratorTool::generate.
   */
  virtual bool generate( const unsigned int nPileUp , 
                         LHCb::HepMCEvents * theEvents ,
                         LHCb::GenCollisions * theCollisions ) ;

  /// Implements ISampleGeneratorTool::printCounters
  virtual void printCounters( ) const { ; } 
};
#endif // GENERATORS_MINIMUMBIAS_H
