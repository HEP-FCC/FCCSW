// $Id: CollidingBeams.h,v 1.3 2005-12-31 17:32:01 robbep Exp $
#ifndef GENERATORS_COLLIDINGBEAMS_H 
#define GENERATORS_COLLIDINGBEAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IBeamTool.h"

// Forward declarations
class IRndmGenSvc ;

/** @class CollidingBeams CollidingBeams.h "CollidingBeams.h"
 *  
 *  Tool to compute colliding beams values. Concrete implementation
 *  of a beam tool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class CollidingBeams : public GaudiTool, virtual public IBeamTool {
 public:
  /// Standard constructor
  CollidingBeams( const std::string& type, const std::string& name,
                  const IInterface* parent ) ;
  
  virtual ~CollidingBeams( ); ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize( ) ;  
  
  /** Implements IBeamTool::getMeanBeams
   */
  virtual void getMeanBeams( Gaudi::XYZVector & pBeam1 , 
                             Gaudi::XYZVector & pBeam2 ) const ;
  
  /** Implements IBeamTool::getBeams
   *  Compute beam 3-momentum taking into account the horizontal and vertical
   *  beam angles (given by job options). These angles are Gaussian-smeared
   *  with an angular smearing equal to (emittance/beta*)^1/2.
   */
  virtual void getBeams( Gaudi::XYZVector & pBeam1 , 
                         Gaudi::XYZVector & pBeam2 ) ;

 private:
  std::string m_beamParameters ; ///< Location of beam parameters (set by options)
 
  Rndm::Numbers m_gaussianDist ; ///< Gaussian random number generator
};
#endif // GENERATORS_COLLIDINGBEAMS_H
