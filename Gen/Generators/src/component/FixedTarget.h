// $Id: FixedTarget.h,v 1.3 2005-12-31 17:32:01 robbep Exp $
#ifndef GENERATORS_COLLIDINGBEAMS_H 
#define GENERATORS_COLLIDINGBEAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IBeamTool.h"

// Forward declarations
class IRndmGenSvc ;

/** @class FixedTarget FixedTarget.h "FixedTarget.h"
 *  
 *  Tool to compute beam values with only one beam colliding to a fixed
 *  target. Concrete implementation of a IBeamTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class FixedTarget : public GaudiTool, virtual public IBeamTool {
 public:
  /// Standard constructor
  FixedTarget( const std::string& type , const std::string& name,
               const IInterface* parent ) ;
  
  virtual ~FixedTarget( ); ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize( ) ;  
  
  /// Implements IBeamTool::getMeanBeams. See CollidingBeams::getMeanBeams
  virtual void getMeanBeams( Gaudi::XYZVector & pBeam1 , 
                             Gaudi::XYZVector & pBeam2 ) const ;
  
  /// Implements IBeamTool::getBeams. See CollidingBeams::getBeams
  virtual void getBeams( Gaudi::XYZVector & pBeam1 , 
                         Gaudi::XYZVector & pBeam2 ) ;
  
 private:
  std::string m_beamParameters ; ///< Location of beam parameters (set by options)

  Rndm::Numbers m_gaussianDist ; ///< Gaussian random number generator
};
#endif // GENERATORS_FIXEDTARGET_H
