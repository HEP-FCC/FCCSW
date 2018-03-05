#ifndef RECINTERFACE_IEXTRAPOLATIONTOOL_H
#define RECINTERFACE_IEXTRAPOLATIONTOOL_H

#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "GaudiKernel/IAlgTool.h"
#include "Math/Math.h"

/** @class IExtrapolationTool RecInterface/RecInterface/IExtrapolationTool.h
 * IExtrapolationTool.h
 *
 *  Abstract interface for extrapolation tool to extrapolate a track described
 * by vertex, momentum & charge.
 *
 *  @author Julia Hrdinka
 *  @date 2017-02
 */

class IExtrapolationTool : virtual public IAlgTool {
 public:
  DeclareInterfaceID(IExtrapolationTool, 1, 0);

  /// virtual method to extrapolate a track
  /// @param vertex	a 3D position describing the vertex of the particle
  /// @param momentum a 3D vector describing the momentum of the particle
  /// @param charge the charge of the particle
  virtual Acts::ExtrapolationCell<Acts::TrackParameters> extrapolate(
      const fcc::Vector3D& vertex, const fcc::Vector3D& momentum,
      double charge) = 0;
};

#endif /* RECINTERFACE_IEXTRAPOLATIONTOOL_H */
