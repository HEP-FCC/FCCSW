#ifndef GENERATION_GAUSSSMEARVERTEX_H
#define GENERATION_GAUSSSMEARVERTEX_H

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generation/IVertexSmearingTool.h"

/** @class GaussSmearVertex GaussSmearVertex.h "GaussSmearVertex.h"
 *
 *  Tool to smear vertex with flat smearing along the x- y- and z-axis.
 *  Concrete implementation of a IVertexSmearingTool.
 *
 *  @author Patrick Robbe
 *  @author Daniel Funke
 *  @date   2008-05-18
 */
class GaussSmearVertex : public GaudiTool, virtual public IVertexSmearingTool {
public:
  /// Standard constructor
  GaussSmearVertex(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~GaussSmearVertex();  ///< Destructor

  /// Initialize method
  virtual StatusCode initialize();

  /** Implements IVertexSmearingTool::smearVertex.
   */
  virtual StatusCode smearVertex(HepMC::GenEvent& theEvent);

private:
  /// Minimum value for the x coordinate of the vertex (set by options)
  double m_xsig;

  /// Minimum value for the y coordinate of the vertex (set by options)
  double m_ysig;

  /// Minimum value for the z coordinate of the vertex (set by options)
  double m_zsig;

  /// Direction of the beam to take into account TOF vs nominal IP8, can have
  /// only values -1 or 1, or 0 to switch off the TOF and set time of
  /// interaction to zero (default = 1, as for beam 1)
  int m_zDir;

  Rndm::Numbers m_gaussDistX;
  Rndm::Numbers m_gaussDistY;
  Rndm::Numbers m_gaussDistZ;
};

#endif  // GENERATION_GAUSSSMEARVERTEX_H
