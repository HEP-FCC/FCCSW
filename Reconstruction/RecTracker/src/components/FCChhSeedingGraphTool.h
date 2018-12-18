#ifndef RECTRACKER_FCCHHSEEDINGGRAPHTOOL_H
#define RECTRACKER_FCCHHSEEDINGGRAPHTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ILayerGraphTool.h"

#include "RecTracker/CMGraph.h"


class FCChhSeedingGraphTool : public GaudiTool, virtual public ILayerGraphTool {
public:
  FCChhSeedingGraphTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~FCChhSeedingGraphTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual tricktrack::CMGraph graph() override final;
private:
  Gaudi::Property<std::vector<std::vector<std::string>>> m_layerPaths {this, "layerPaths", {{
  
      {"b1", "b2", "b3", "b4"},

      {"b1", "b2", "b3", "e1"},
      {"b1", "b2", "e1", "e2"},
      {"b1", "e1", "e2", "e3"},
      {"e1", "e2", "e3", "e4"},
      {"e2", "e3", "e4", "e5"},

      {"b1", "b2", "b3", "n1"},
      {"b1", "b2", "n1", "n2"},
      {"b1", "n1", "n2", "n3"},
      {"n1", "n2", "n3", "n4"},
      {"n2", "n3", "n4", "n5"},
  
  }} };
};

#endif /* RECTRACKER_FCCHHSEEDINGGRAPHTOOL_H */
