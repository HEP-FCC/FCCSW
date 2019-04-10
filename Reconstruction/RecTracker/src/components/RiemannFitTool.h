#ifndef RECTRACKER_RIEMANNFITTOOL_H
#define RECTRACKER_RIEMANNFITTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITrackFittingTool.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include <map>

/** @class RiemannFitTool
 * Track fitting tool implementation using tricktracks' Riemannfit
 */
class RiemannFitTool : public GaudiTool, virtual public ITrackFittingTool {
public:
  RiemannFitTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~RiemannFitTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual std::pair<fcc::TrackCollection*, fcc::TrackStateCollection*>
  fitTracks(const fcc::PositionedTrackHitCollection* theHits,
            std::multimap<unsigned int, unsigned int> seedmap) override final;

private:
  Gaudi::Property<double> m_Bz{this, "Bz", 4., "Field strength along Z"};
  Gaudi::Property<double> m_hitRes{this, "hitRes", 1e-8, "Resolution of local hit coordinates"};
  Gaudi::Property<bool> m_doFit{this, "doFit", true, "flag to actually perform the fit"};
  Gaudi::Property<bool> m_calcErrors{this, "calcErrors", true, "flag to actually calculate errors"};
  Gaudi::Property<bool> m_fitOnlyPrimary{this, "fitOnlyPrimary", false, "flag to only fit the particle with trackID 1"};
  Gaudi::Property<bool> m_calcMultipleScattering{this, "MultipleScatteringErrors", false,
                                                 "flag to toggle estimation of multiple scattering errors"};
};

#endif /* RECTRACKER_RIEMANNFITTOOL_H */
