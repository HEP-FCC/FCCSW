// $Id: UniformSmearVertex.h,v 1.2 2008-05-06 08:21:00 gcorti Exp $
#ifndef GENERATORS_UNIFORMSMEARVERTEX_H 
#define GENERATORS_UNIFORMSMEARVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 

#include "Generators/IVertexSmearingTool.h"

/** @class UniformSmearVertex UniformSmearVertex.h "UniformSmearVertex.h"
 *  
 *  Tool to smear vertex with flat distribution along the z-axis and 
 *  in the xy plane (within a radius). Simple modification of Patrick
 *  Robbe's FlatZSmearVertex.
 *  Concrete implementation of a IVertexSmearingTool.
 * 
 *  @author Massi Ferro-Luzzi
 *  @date   2007-09-07
 */
class UniformSmearVertex : public GaudiTool, 
                           virtual public IVertexSmearingTool {
 public:
  /// Standard constructor
  UniformSmearVertex( const std::string& type , const std::string& name,
                      const IInterface* parent ) ;
  
  virtual ~UniformSmearVertex( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /** Implements IVertexSmearingTool::smearVertex.
   *  Does the same than BeamSpotSmearVertex::smearVertex for the x and y
   *  direction but generates flat distribution for the z-coordinate of
   *  the primary vertex.
   */
  virtual StatusCode smearVertex( LHCb::HepMCEvent * theEvent ) ;
  
 private:
  /// Maximum value for the r coordinate of the vertex (set by options)
  double m_rmax   ;
  
  /// Minimum value for the z coordinate of the vertex (set by options)
  double m_zmin   ;
  
  /// Maximum value for the z coordinate of the vertex (set by options)
  double m_zmax   ;

  /// Square of m_rmax                     (set at initialisation)
  double m_rmaxsq ;

  /// Difference between m_zmax and m_zmin (set at initialisation)
  double m_deltaz ;

  /// Direction of the beam to take into account TOF vs nominal IP8, can have
  /// only values -1 or 1, or 0 to switch off the TOF and set time of 
  /// interaction to zero (default = 1, as for beam 1)
  int m_zDir;

  Rndm::Numbers m_flatDist ; ///< Flat random number generator
};
#endif // GENERATORS_UNIFORMSMEARVERTEX_H
