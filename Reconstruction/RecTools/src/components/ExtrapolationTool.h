#ifndef RECTRACKER_EXTRAPOLATIONTOOL_H
#define RECTRACKER_EXTRAPOLATIONTOOL_H

#include "DetInterface/ITrackingGeoSvc.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagneticField/IBFieldSvc.h"
#include "Math/Math.h"
#include "RecInterface/IExtrapolationTool.h"

/** @class ExtrapolationTool
 *
 *  Realisation of the extrapolation tool to extrapolate a track described
 * by vertex, momentum & charge through the tracking geometry, taking the given magnetic field into account. It
 * internally uses the ACTS <a
 * href="http://acts.web.cern.ch/ACTS/latest/doc/classActs_1_1ExtrapolationEngine.html">ExtrapolationEngine</a>
 * and returns a <a
 * href="http://acts.web.cern.ch/ACTS/latest/doc/classActs_1_1ExtrapolationCell.html</a>.
 *
 *  @author Julia Hrdinka
 *  @date   2017-02
 */

namespace Acts {
class ExtrapolationEngine;
}

class ExtrapolationTool : public GaudiTool, virtual public IExtrapolationTool {
public:
  /// Constructor
  ExtrapolationTool(const std::string& type, const std::string& name, const IInterface* parent);
  /// Destructor
  ~ExtrapolationTool() = default;
  /// Gaudi interface initialization method
  /// it initializes the extrapolation engine
  virtual StatusCode initialize() final;
  /// Gaudi interface finalize method
  virtual StatusCode finalize() final;
  /// Extrapolation method
  /// extrapolates a track through the tracking geometry
  /// beginning at a given vertex into a given direction with given momentum and
  /// charge.
  /// @param vertex the origin position of the particle
  /// @param momentum the three dimensional momentum of the particle
  /// @param charge the charge of the particle
  /// @return the Acts::ExtrapolationCell
  virtual Acts::ExtrapolationCell<Acts::TrackParameters>
  extrapolate(const fcc::Vector3D& vertex, const fcc::Vector3D& momentum, double charge) final;

private:
  /// The tracking geometry service
  ServiceHandle<ITrackingGeoSvc> m_trkGeoSvc;
  /// The magnetic field service
  ServiceHandle<IBFieldSvc> m_bFieldSvc;
  /// switch if sensitive hits should be collected
  bool m_collectSensitive;
  /// switch if hits on passive material should be collected
  bool m_collectPassive;
  /// switch if hits on boundaries should be collected
  bool m_collectBoundary;
  /// switch if material should be collected along the way
  bool m_collectMaterial;
  /// stay with curvilinear parameters for sensitive mode
  bool m_sensitiveCurvilinear;
  /// depth of search applied
  int m_searchMode;
  /// the given path limit (-1 if no limit)
  double m_pathLimit;

  /// the extrapolation engine
  std::shared_ptr<Acts::ExtrapolationEngine> m_extrapolationEngine;
};

#endif /* RECTRACKER_EXTRAPOLATIONTOOL_H */
