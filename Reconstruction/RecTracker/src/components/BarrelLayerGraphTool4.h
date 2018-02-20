#ifndef RECTRACKER_BARRELLAYERGRAPHTOOL_H
#define RECTRACKER_BARRELLAYERGRAPHTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ILayerGraphTool.h"

#include "tricktrack/CMGraph.h"


class BarrelLayerGraphTool4 : public GaudiTool, virtual public ILayerGraphTool {
public:
  BarrelLayerGraphTool4(const std::string& type, const std::string& name, const IInterface* parent);
  ~BarrelLayerGraphTool4() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual tricktrack::CMGraph getGraph() override final;
};

#endif /* RECTRACKER_BARRELLAYERGRAPHTOOL_H */
