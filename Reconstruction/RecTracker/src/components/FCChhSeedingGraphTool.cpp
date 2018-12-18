#include "FCChhSeedingGraphTool.h"

#include "RecInterface/ILayerGraphTool.h"
#include "RecTracker/TrickTrackGraphUtility.h"




DECLARE_TOOL_FACTORY(FCChhSeedingGraphTool)

FCChhSeedingGraphTool::FCChhSeedingGraphTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ILayerGraphTool>(this);
}

StatusCode FCChhSeedingGraphTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  return sc;
}

StatusCode FCChhSeedingGraphTool::finalize() { return GaudiTool::finalize(); }

/// Construct and return the layergraph for the Cellular automaton,
tricktrack::CMGraph FCChhSeedingGraphTool::graph() {

  auto g = tricktrack::createGraph2(m_layerPaths);

  //log debug info for graph layer

  debug() << "CMGraph Layers: ";
  for (auto layer: g.theLayers) {
    debug() << layer.name() << "\t";
  }
  debug() << endmsg;

  return g;
  }

