// $Id: FlatZSmearVertex.h,v 1.5 2010-05-09 17:05:30 gcorti Exp $
#ifndef GENERATORS_FLATZSMEARVERTEX_H 
#define GENERATORS_FLATZSMEARVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 

#include "Generators/IVertexSmearingTool.h"

/** @class FlatZSmearVertex FlatZSmearVertex.h "FlatZSmearVertex.h"
 *  
 *  Tool to smear vertex with flat smearing along the z-axis and Gaussian
 *  smearing for the other axis (as in BeamSpotSmearVertex). Concrete
 *  implementation of a IVertexSmearingTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class FlatZSmearVertex : public GaudiTool, virtual public IVertexSmearingTool {
 public:
  /// Standard constructor
  FlatZSmearVertex( const std::string& type , const std::string& name,
                    const IInterface* parent ) ;
  
  virtual ~FlatZSmearVertex( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /** Implements IVertexSmearingTool::smearVertex.
   *  Does the same than BeamSpotSmearVertex::smearVertex for the x and y
   *  direction but generates flat distribution for the z-coordinate of
   *  the primary vertex.
   */
  virtual StatusCode smearVertex( LHCb::HepMCEvent * theEvent ) ;
  
 private:
  std::string m_beamParameters ; ///< Location of beam parameters (set by options)  

  /// Number of sigma above which to cut for x-axis smearing (set by options)
  double m_xcut   ;

  /// Number of sigma above which to cut for y-axis smearing (set by options)
  double m_ycut   ;

  /// Minimum value for the z coordinate of the vertex (set by options)
  double m_zmin   ;
  
  /// Maximum value for the z coordinate of the vertex (set by options)
  double m_zmax   ;

  /// Direction of the beam to take into account TOF vs nominal IP8, can have
  /// only values -1 or 1, or 0 to switch off the TOF and set time of 
  /// interaction to zero (default = 1, as for beam 1)
  int m_zDir;

  Rndm::Numbers m_gaussDist ; ///< Gaussian random number generator

  Rndm::Numbers m_flatDist ; ///< Flat random number generator
};
#endif // GENERATORS_FLATZSMEARVERTEX_H
