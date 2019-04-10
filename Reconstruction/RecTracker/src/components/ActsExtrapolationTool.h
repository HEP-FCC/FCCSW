#ifndef RECTRACKER_ACTSEXTRAPOLATIONTOOL_H
#define RECTRACKER_ACTSEXTRAPOLATIONTOOL_H

#include "DetInterface/ITrackingGeoSvc.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "RecInterface/ITrackExtrapolationTool.h"

#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Extrapolation/ExtrapolationCell.hpp"

/** @class ActsExtrapolationTool
 *
 *  Realisation of the extrapolation tool to extrapolate a track
 */

namespace Acts {
class ExtrapolationEngine;
}

class ActsExtrapolationTool : public GaudiTool, virtual public ITrackExtrapolationTool {
public:
  /// Constructor
  ActsExtrapolationTool(const std::string& type, const std::string& name, const IInterface* parent);
  /// Destructor
  ~ActsExtrapolationTool() = default;
  /// Gaudi interface initialization method
  /// it initializes the extrapolation engine
  virtual StatusCode initialize() final;
  /// Gaudi interface finalize method
  virtual StatusCode finalize() final;
  /// Extrapolation method
  /// extrapolates a track through the tracking geometry
  /// beginning at a given vertex into a given direction with given momentum and
  /// charge.
  virtual std::vector<fcc::TrackState> extrapolate(const fcc::TrackState theTrackState) final;

  /// convert ACTS track parameters to an ACTS ExtrapolationCell
  /// configures several extrapolation flags
  Acts::ExtrapolationCell<Acts::TrackParameters> getExtrapolationCell(const Acts::BoundParameters startParameters);

private:
  /// The tracking geometry service
  ServiceHandle<ITrackingGeoSvc> m_trkGeoSvc;
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
  /// for the constant magnetic field: field strength in z
  Gaudi::Property<double> m_bFieldZ{this, "bFieldZ", 4.};

  /// the extrapolation engine
  std::shared_ptr<Acts::ExtrapolationEngine> m_extrapolationEngine;
};

#endif /* RECTRACKER_ACTSEXTRAPOLATIONTOOL_H */
