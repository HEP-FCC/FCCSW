#ifndef RECTRACKER_RIEMANNFITTOOL_H
#define RECTRACKER_RIEMANNFITTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITrackFittingTool.h"
#include "RecInterface/IDoubletCreationTool.h"
#include "RecInterface/ILayerGraphTool.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"






class RiemannFitTool : public GaudiTool, virtual public ITrackFittingTool {
public:
  RiemannFitTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~RiemannFitTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;


  virtual fcc::TrackState fitTrack(const fcc::PositionedTrackHitCollection* theHits, std::vector<unsigned int> hitIndices) override final;

private:
};

#endif /* RECTRACKER_RIEMANNFITTOOL_H */
