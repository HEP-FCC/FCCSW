#ifndef RECTRACKER_FULLINNERLAYERGRAPHTOOL_H
#define RECTRACKER_FULLINNERLAYERGRAPHTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ILayerGraphTool.h"

#include "tricktrack/CMGraph.h"


class FullInnerLayerGraphTool : public GaudiTool, virtual public ILayerGraphTool {
public:
  FullInnerLayerGraphTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~FullInnerLayerGraphTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual tricktrack::CMGraph getGraph() override final;
};

#endif /* RECTRACKER_FULLINNERLAYERGRAPHTOOL_H */
