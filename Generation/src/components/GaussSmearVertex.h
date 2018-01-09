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
  Gaudi::Property<double> m_xsig{this, "xVertexSigma", 0.0 * Gaudi::Units::mm, "Spread of x coordinate"};
  Gaudi::Property<double> m_ysig{this, "yVertexSigma", 0.0 * Gaudi::Units::mm, "Spread of y coordinate"};
  Gaudi::Property<double> m_zsig{this, "zVertexSigma", 0.0 * Gaudi::Units::mm, "Spread of z coordinate"};
  Gaudi::Property<double> m_tsig{this, "tVertexSigma", 0.0 * Gaudi::Units::mm, "Spread of t coordinate"};

  Gaudi::Property<double> m_xmean{this, "xVertexMean", 0.0 * Gaudi::Units::mm, "Mean of x coordinate"};
  Gaudi::Property<double> m_ymean{this, "yVertexMean", 0.0 * Gaudi::Units::mm, "Mean of y coordinate"};
  Gaudi::Property<double> m_zmean{this, "zVertexMean", 0.0 * Gaudi::Units::mm, "Mean of z coordinate"};
  Gaudi::Property<double> m_tmean{this, "tVertexMean", 0.0 * Gaudi::Units::mm, "Mean of t coordinate"};

  Rndm::Numbers m_gaussDist;
};

#endif  // GENERATION_GAUSSSMEARVERTEX_H
