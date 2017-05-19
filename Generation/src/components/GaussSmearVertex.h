#ifndef GENERATION_GAUSSSMEARVERTEX_H
#define GENERATION_GAUSSSMEARVERTEX_H

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generation/IVertexSmearingTool.h"

/** @class GaussSmearVertex GaussSmearVertex.h "GaussSmearVertex.h"
 *
 *  Tool to smear vertex with gaussian smearing along the x- y- z- and t-axis.
 *  Concrete implementation of a IVertexSmearingTool.
 *
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
  double m_xsig;
  double m_xmean;

  double m_ysig;
  double m_ymean;

  double m_zsig;
  double m_zmean;

  double m_tsig;
  double m_tmean;
  
  /// Minimum value for the x coordinate of the vertex (set by options)
  Gaudi::Property<double> m_xmin{this, "xVertexMin", 0.0 * Gaudi::Units::mm, "Min value for x coordinate"};

  /// Minimum value for the y coordinate of the vertex (set by options)
  Gaudi::Property<double> m_ymin{this, "yVertexMin", 0.0 * Gaudi::Units::mm, "Min value for y coordinate"};

  /// Minimum value for the z coordinate of the vertex (set by options)
  Gaudi::Property<double> m_zmin{this, "zVertexMin", 0.0 * Gaudi::Units::mm, "Min value for z coordinate"};

  /// Maximum value for the x coordinate of the vertex (set by options)
  Gaudi::Property<double> m_xmax{this, "xVertexMax", 0.0 * Gaudi::Units::mm, "Max value for x coordinate"};

  /// Maximum value for the y coordinate of the vertex (set by options)
  Gaudi::Property<double> m_ymax{this, "yVertexMax", 0.0 * Gaudi::Units::mm, "Max value for y coordinate"};

  /// Maximum value for the z coordinate of the vertex (set by options)
  Gaudi::Property<double> m_zmax{this, "zVertexMax", 0.0 * Gaudi::Units::mm, "Max value for z coordinate"};

  /// Maximum value for the t coordinate of the vertex (set by options)
  Gaudi::Property<double> m_tmax{this, "tVertexMax", 0.0 * Gaudi::Units::mm / Gaudi::Units::c_light, "Max value for t coordinate"};

  /// Minimum value for the t coordinate of the vertex (set by options)
  Gaudi::Property<double> m_tmin{this, "tVertexMin", 0.0 * Gaudi::Units::mm / Gaudi::Units::c_light, "Min value for t coordinate"};

  /// Direction of the beam to take into account TOF vs nominal IP8, can have
  /// only values -1 or 1, or 0 to switch off the TOF and set time of
  /// interaction to zero (default = 1, as for beam 1)
  Gaudi::Property<int> m_zDir{"beamDirection", 0, "Direction of the beam, possible values: -1, 1 or 0"};

  Rndm::Numbers m_gaussDist;
};

#endif  // GENERATION_GAUSSSMEARVERTEX_H
